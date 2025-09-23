#include "ControllerIconManagerSubsystem.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Styling/SlateBrush.h"
#include "Engine/World.h"
#include "UObject/SoftObjectPath.h"
#include "Misc/CoreDelegates.h"
#include "Framework/Application/SlateApplication.h"
#include "InputCoreTypes.h"
#include "Engine/AssetManager.h"         // UAssetManager
#include "Engine/StreamableManager.h"    // FStreamableManager::LoadSynchronous
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerController.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

#if __has_include("CommonInputSubsystem.h")
#include "CommonInputSubsystem.h"
#define HAS_COMMONUI 1
#else
#define HAS_COMMONUI 0
#endif

// Log auxiliar
static void LogIconSet(const TCHAR* Where, EIconSet Set)
{
	UE_LOG(LogTemp, Log, TEXT("[ControllerIcons][%s] IconSet=%d (0=PC,1=PS5,2=XBOX)"), Where, (int32)Set);
}

void UControllerIconManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Nombres a sustituir (los 5 que usa tu HUD)
	AllowedNames = {
		FName("Bumper_R"),   // R1 / RB
		FName("Trigger_R"),  // R2 / RT
		FName("Trigger_L"),  // L2 / LT
		FName("FaceRight"),  // ○ / B
		FName("FaceUp"),      // △ / Y
		FName("FaceLeft"),  // ← NUEVO (PS5 = □, Xbox = X, PC = E)
		FName("DPad_Up"),
		FName("DPad_Down"),
		FName("Bumper_L"),      // ← NUEVO: PC=click derecho, Xbox=LB, PS5=L1
		FName("Bumper_R2")
	};

	// Carpetas por plataforma (tolerante a may/min)
	FolderNamesPerSet.Add(EIconSet::PS5, { TEXT("Ps5"),  TEXT("ps5"),  TEXT("PS5") });
	FolderNamesPerSet.Add(EIconSet::XBOX, { TEXT("Xbox"), TEXT("xbox"), TEXT("XBOX") });
	FolderNamesPerSet.Add(EIconSet::PC, { TEXT("Pc"),   TEXT("pc"),   TEXT("PC") });

	// Re-escaneo al cargar mapa
	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(
		this, &UControllerIconManagerSubsystem::OnPostLoadMap);

	// Set inicial “seguro” (PC) antes de tener info real
	SetIconSet(EIconSet::PC);

	BindInputDetection();

	// Permite forzar por CLI (útil para probar en Shipping): -forceiconset=pc|xbox|ps5
	{
		FString Force;
		if (FParse::Value(FCommandLine::Get(), TEXT("forceiconset="), Force))
		{
			Force = Force.ToLower();
			if (Force == TEXT("pc"))   SetIconSet(EIconSet::PC);
			if (Force == TEXT("xbox")) SetIconSet(EIconSet::XBOX);
			if (Force == TEXT("ps5"))  SetIconSet(EIconSet::PS5);
			LogIconSet(TEXT("CmdLine"), CurrentSet);
		}
	}

	// Reintentos de detección inicial (prioriza mando si existe)
	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateWeakLambda(this, [this](float) { EvaluateInitialDevice(); return false; }),
		0.10f);
	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateWeakLambda(this, [this](float) { EvaluateInitialDevice(); return false; }),
		0.50f);
	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateWeakLambda(this, [this](float) { EvaluateInitialDevice(); return false; }),
		1.50f);

	// Escaneo periódico de widgets NUEVOS (cada 0.5s)
	NewWidgetScanTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(this, &UControllerIconManagerSubsystem::TickScanNewWidgets),
		0.5f);

	if (UWorld* World = GetWorld())
	{
		ApplyIconSetToAllWidgets(World);
		LogIconSet(TEXT("InitApply"), CurrentSet);
	}
}

void UControllerIconManagerSubsystem::Deinitialize()
{
	UnbindInputDetection();

	if (NewWidgetScanTickerHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(NewWidgetScanTickerHandle);
		NewWidgetScanTickerHandle.Reset();
	}
	SeenWidgets.Empty();

	if (PostLoadMapHandle.IsValid())
	{
		FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);
	}
	Super::Deinitialize();
}

void UControllerIconManagerSubsystem::BindInputDetection()
{
#if HAS_COMMONUI
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULocalPlayer* LP = GI->GetFirstGamePlayer())
		{
			if (UCommonInputSubsystem* CIS = ULocalPlayer::GetSubsystem<UCommonInputSubsystem>(LP))
			{
				// Cambio teclado/gamepad
				CIS->OnInputMethodChangedNative.AddLambda([this](ECommonInputType NewType)
					{
						if (NewType == ECommonInputType::MouseAndKeyboard) OnInputChangedToKeyboard();
						else OnInputChangedToGamepadGeneric();
					});

				// Tipo de gamepad
				CIS->OnGamepadInputTypeChangedNative.AddLambda([this](ECommonGamepadType T)
					{
						if (T == ECommonGamepadType::Playstation) OnGamepadTypePlayStation();
						else if (T == ECommonGamepadType::Xbox)  OnGamepadTypeXbox();
						else OnInputChangedToGamepadGeneric();
					});

				// Estado inicial
				if (CIS->GetCurrentInputType() == ECommonInputType::MouseAndKeyboard)
				{
					OnInputChangedToKeyboard();
				}
				else
				{
					switch (CIS->GetCurrentGamepadType())
					{
					case ECommonGamepadType::Playstation: OnGamepadTypePlayStation(); break;
					case ECommonGamepadType::Xbox:        OnGamepadTypeXbox();        break;
					default:                               OnInputChangedToGamepadGeneric(); break;
					}
				}
			}
		}
	}
#endif

	// UE 5.2: evento de conexión (útil en Shipping)
#if (ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION <= 2)
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		ControllerConnHandle = FCoreDelegates::OnControllerConnectionChange.AddLambda(
			[this](bool bIsConnected, auto /*UserId*/, int32 ControllerId)
			{
				if (bIsConnected)
				{
					ActiveControllers.Add(ControllerId);
					OnInputChangedToGamepadGeneric();
				}
				else
				{
					ActiveControllers.Remove(ControllerId);
					if (ActiveControllers.Num() == 0) OnInputChangedToKeyboard();
				}
			}
		);
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
#endif

		// Polling robusto (Shipping) — también detecta el primer input
		PollTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
			FTickerDelegate::CreateUObject(this, &UControllerIconManagerSubsystem::TickPoll),
			0.25f
		);

	// Estado inicial por adjuntos de Slate
	const bool bAttached = FSlateApplication::IsInitialized() && FSlateApplication::Get().IsGamepadAttached();
	bLastPolledGamepadAttached = bAttached;
	if (bAttached) OnInputChangedToGamepadGeneric(); else OnInputChangedToKeyboard();
}

void UControllerIconManagerSubsystem::UnbindInputDetection()
{
	if (PollTickerHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(PollTickerHandle);
		PollTickerHandle.Reset();
	}

#if (ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION <= 2)
	if (ControllerConnHandle.IsValid())
	{
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
			FCoreDelegates::OnControllerConnectionChange.Remove(ControllerConnHandle);
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
	}
	ActiveControllers.Empty();
#endif
}

bool UControllerIconManagerSubsystem::HasAnyGamepadAttached() const
{
	const bool bSlateSays = FSlateApplication::IsInitialized() && FSlateApplication::Get().IsGamepadAttached();
#if (ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION <= 2)
	const bool bCoreDelegateSays = ActiveControllers.Num() > 0;
#else
	const bool bCoreDelegateSays = false;
#endif
	return bSlateSays || bCoreDelegateSays;
}

void UControllerIconManagerSubsystem::EvaluateInitialDevice()
{
#if HAS_COMMONUI
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULocalPlayer* LP = GI->GetFirstGamePlayer())
		{
			if (UCommonInputSubsystem* CIS = ULocalPlayer::GetSubsystem<UCommonInputSubsystem>(LP))
			{
				// Si hay teclado activo pero existe mando → prioriza mando
				if (CIS->GetCurrentInputType() == ECommonInputType::MouseAndKeyboard && HasAnyGamepadAttached())
				{
					switch (CIS->GetCurrentGamepadType())
					{
					case ECommonGamepadType::Playstation: OnGamepadTypePlayStation(); break;
					case ECommonGamepadType::Xbox:        OnGamepadTypeXbox();        break;
					default:                               OnInputChangedToGamepadGeneric(); break;
					}
					return;
				}
				// Si ya está en modo mando, respeta marca si la conoce
				else if (CIS->GetCurrentInputType() != ECommonInputType::MouseAndKeyboard)
				{
					switch (CIS->GetCurrentGamepadType())
					{
					case ECommonGamepadType::Playstation: OnGamepadTypePlayStation(); break;
					case ECommonGamepadType::Xbox:        OnGamepadTypeXbox();        break;
					default:                               OnInputChangedToGamepadGeneric(); break;
					}
					return;
				}
			}
		}
	}
#endif

	// Sin CommonUI / no disponible: usa adjuntos. Si hay mando → XBOX genérico
	if (HasAnyGamepadAttached())
	{
		OnInputChangedToGamepadGeneric();
	}
	else
	{
		OnInputChangedToKeyboard();
	}
}

static bool Local_AnyKeyDown(APlayerController* PC, const TArray<FKey>& Keys)
{
	if (!PC) return false;
	for (const FKey& K : Keys)
	{
		if (PC->IsInputKeyDown(K)) return true;
	}
	return false;
}

bool UControllerIconManagerSubsystem::TickPoll(float /*DeltaTime*/)
{
	// Hot-plug por Slate/Core
	const bool bAttachedNow = HasAnyGamepadAttached();
	if (bAttachedNow != bLastPolledGamepadAttached)
	{
		bLastPolledGamepadAttached = bAttachedNow;
		if (bAttachedNow) OnInputChangedToGamepadGeneric();
		else              OnInputChangedToKeyboard();
		LogIconSet(TEXT("HotPlug"), CurrentSet);
	}

	// Detección por teclas (teclado NO gana si hay mando conectado)
	UWorld* World = GetWorld();
	if (!World) return true;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC) return true;

	static const TArray<FKey> GamepadKeys = {
		EKeys::Gamepad_FaceButton_Bottom, EKeys::Gamepad_FaceButton_Right,
		EKeys::Gamepad_FaceButton_Top,    EKeys::Gamepad_FaceButton_Left,
		EKeys::Gamepad_LeftShoulder,      EKeys::Gamepad_RightShoulder,
		EKeys::Gamepad_LeftTrigger,       EKeys::Gamepad_RightTrigger,
		EKeys::Gamepad_Special_Left,      EKeys::Gamepad_Special_Right,
		EKeys::Gamepad_LeftStick_Up,      EKeys::Gamepad_LeftStick_Down,
		EKeys::Gamepad_LeftStick_Left,    EKeys::Gamepad_LeftStick_Right,
		EKeys::Gamepad_RightStick_Up,     EKeys::Gamepad_RightStick_Down,
		EKeys::Gamepad_RightStick_Left,   EKeys::Gamepad_RightStick_Right
	};

	static const TArray<FKey> KBMKeys = {
		EKeys::LeftMouseButton, EKeys::RightMouseButton, EKeys::MiddleMouseButton,
		EKeys::W, EKeys::A, EKeys::S, EKeys::D,
		EKeys::SpaceBar, EKeys::LeftShift, EKeys::RightShift,
		EKeys::Enter, EKeys::Escape
	};

	if (Local_AnyKeyDown(PC, GamepadKeys))
	{
		OnInputChangedToGamepadGeneric();
		LogIconSet(TEXT("KeyPollGamepad"), CurrentSet);
	}
	else if (!bAttachedNow && Local_AnyKeyDown(PC, KBMKeys)) // teclado solo si NO hay mando
	{
		OnInputChangedToKeyboard();
		LogIconSet(TEXT("KeyPollKBM"), CurrentSet);
	}

	return true;
}

void UControllerIconManagerSubsystem::OnPostLoadMap(UWorld* LoadedWorld)
{
	ApplyIconSetToAllWidgets(LoadedWorld);

	// Re-escaneos diferidos por si HUDs se crean tarde (y reevaluar dispositivo)
	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateLambda([this, LoadedWorld](float)
			{
				ApplyIconSetToAllWidgets(LoadedWorld);
				EvaluateInitialDevice();
				return false;
			}), 0.25f);

	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateLambda([this, LoadedWorld](float)
			{
				ApplyIconSetToAllWidgets(LoadedWorld);
				EvaluateInitialDevice();
				return false;
			}), 1.0f);

	// Extra tardío para máquinas lentas/Shipping
	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateLambda([this, LoadedWorld](float)
			{
				ApplyIconSetToAllWidgets(LoadedWorld);
				EvaluateInitialDevice();
				return false;
			}), 2.5f);
}

void UControllerIconManagerSubsystem::OnInputChangedToKeyboard()
{
	// Preferimos mando: NO volver a PC si hay uno conectado
	if (HasAnyGamepadAttached())
	{
		return;
	}
	SetIconSet(EIconSet::PC);
	if (UWorld* World = GetWorld()) ApplyIconSetToAllWidgets(World);
}

void UControllerIconManagerSubsystem::OnInputChangedToGamepadGeneric()
{
	SetIconSet(EIconSet::XBOX); // genérico si no sabemos marca
	if (UWorld* World = GetWorld()) ApplyIconSetToAllWidgets(World);
}

void UControllerIconManagerSubsystem::OnGamepadTypePlayStation()
{
	SetIconSet(EIconSet::PS5);
	if (UWorld* World = GetWorld()) ApplyIconSetToAllWidgets(World);
}

void UControllerIconManagerSubsystem::OnGamepadTypeXbox()
{
	SetIconSet(EIconSet::XBOX);
	if (UWorld* World = GetWorld()) ApplyIconSetToAllWidgets(World);
}

void UControllerIconManagerSubsystem::SetIconSet(EIconSet NewSet)
{
	if (CurrentSet == NewSet) return;
	CurrentSet = NewSet;
	ClearCache();          // limpiar caché al cambiar familia (PS5/XBOX/PC)
	SeenWidgets.Empty();   // re-procesa widgets actuales en el próximo tick de "nuevos"
}

void UControllerIconManagerSubsystem::ClearCache()
{
	LoadedCache.Empty();
}

void UControllerIconManagerSubsystem::ForceRescan(UObject* WorldContextObject)
{
	if (!WorldContextObject) return;
	if (UWorld* World = WorldContextObject->GetWorld())
	{
		ApplyIconSetToAllWidgets(World);
	}
}

// ========== APLICACIÓN SEGURA A WIDGETS (UMG APIs) ==========
void UControllerIconManagerSubsystem::ApplyIconSetToAllWidgets(UWorld* World)
{
	if (!World) return;

	TArray<UUserWidget*> AllWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(World, AllWidgets, UUserWidget::StaticClass(), false);

	for (UUserWidget* UW : AllWidgets)
	{
		ApplyIconSetToSingleWidget(UW);
	}
}

/** Aplica el set SOLO a un widget concreto (útil para widgets recién creados). */
void UControllerIconManagerSubsystem::ApplyIconSetToSingleWidget(UUserWidget* UW)
{
	if (!IsValid(UW) || !UW->WidgetTree) return;

	TArray<UWidget*> Widgets;
	UW->WidgetTree->GetAllWidgets(Widgets);

	for (UWidget* W : Widgets)
	{
		// UImage: API segura
		if (UImage* Img = Cast<UImage>(W))
		{
			UTexture2D* OldTex = Cast<UTexture2D>(Img->GetBrush().GetResourceObject());
			if (!IsValid(OldTex)) continue;
			if (!IsAllowedName(OldTex->GetName())) continue;

			if (UTexture2D* NewTex = ResolveVariantTextureFromOld(OldTex))
			{
				Img->SetBrushFromTexture(NewTex, /*bMatchSize=*/true);
			}
		}
		// UButton: sustituimos sólo si el brush usa UTexture2D
		else if (UButton* Btn = Cast<UButton>(W))
		{
			FButtonStyle Style = Btn->GetStyle();

			auto ReplaceBrushTex = [this](FSlateBrush& Brush)
				{
					if (UTexture2D* OldTex = Cast<UTexture2D>(Brush.GetResourceObject()))
					{
						if (!IsAllowedName(OldTex->GetName())) return;

						if (UTexture2D* NewTex = ResolveVariantTextureFromOld(OldTex))
						{
							Brush.SetResourceObject(NewTex);
							if (Brush.ImageSize.IsNearlyZero())
							{
								Brush.ImageSize = FVector2D(NewTex->GetSizeX(), NewTex->GetSizeY());
							}
						}
					}
				};

			ReplaceBrushTex(Style.Normal);
			ReplaceBrushTex(Style.Hovered);
			ReplaceBrushTex(Style.Pressed);
			ReplaceBrushTex(Style.Disabled);

			Btn->SetStyle(Style);
		}
	}
}

void UControllerIconManagerSubsystem::MaybeReplaceBrush(FSlateBrush& Brush)
{
	// (Compatibilidad si lo llamas en otro sitio; no se usa con UImage)
	if (UTexture2D* OldTex = Cast<UTexture2D>(Brush.GetResourceObject()))
	{
		if (!IsAllowedName(OldTex->GetName())) return;

		if (UTexture2D* NewTex = ResolveVariantTextureFromOld(OldTex))
		{
			Brush.SetResourceObject(NewTex);
			if (Brush.ImageSize.IsNearlyZero())
			{
				Brush.ImageSize = FVector2D(NewTex->GetSizeX(), NewTex->GetSizeY());
			}
		}
	}
}

const TArray<FString>& UControllerIconManagerSubsystem::GetFolderCandidates() const
{
	return FolderNamesPerSet[CurrentSet];
}

// ========== CARGA SEGURA + CACHÉ ==========
UObject* UControllerIconManagerSubsystem::TryLoadVariantObject(const FString& AssetName)
{
	// Cache hit
	if (const TObjectPtr<UTexture2D>* Found = LoadedCache.Find(FName(*AssetName)))
	{
		return Found->Get();
	}

	// /Game/Assets2D/icon_PS4_PS5/<Folder>/<AssetName>.<AssetName>
	for (const FString& Folder : GetFolderCandidates())
	{
		const FString ObjectPath = FString::Printf(
			TEXT("%s/%s/%s.%s"), *BaseRoot, *Folder, *AssetName, *AssetName);

		const FSoftObjectPath SoftPath(ObjectPath);
		if (!SoftPath.IsValid()) continue;

		FStreamableManager& SM = UAssetManager::GetStreamableManager();
		if (UObject* Obj = SM.LoadSynchronous(SoftPath, /*bManageActiveHandle=*/false))
		{
			if (UTexture2D* AsTex = Cast<UTexture2D>(Obj))
			{
				LoadedCache.Add(FName(*AssetName), TObjectPtr<UTexture2D>(AsTex)); // keep-alive
				return AsTex;
			}
			return Obj;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[ControllerIcons] No se encontró '%s' en set %d"), *AssetName, (int32)CurrentSet);
	return nullptr;
}

UTexture2D* UControllerIconManagerSubsystem::ResolveVariantTextureFromOld(UTexture2D* OldTex)
{
	if (!IsValid(OldTex)) return nullptr;

	if (UObject* Obj = TryLoadVariantObject(OldTex->GetName()))
	{
		if (UTexture2D* NewTex = Cast<UTexture2D>(Obj))
		{
			if (IsValid(NewTex)) return NewTex;
		}
	}
	return nullptr;
}

bool UControllerIconManagerSubsystem::IsAllowedName(const FString& AssetName) const
{
	return AllowedNames.Contains(FName(*AssetName));
}

/** Ticker que aplica el set a widgets NUEVOS que hayan aparecido desde el último scan. */
bool UControllerIconManagerSubsystem::TickScanNewWidgets(float /*DeltaTime*/)
{
	UWorld* World = GetWorld();
	if (!World) return true;

	TArray<UUserWidget*> AllWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(World, AllWidgets, UUserWidget::StaticClass(), false);

	for (UUserWidget* UW : AllWidgets)
	{
		if (!IsValid(UW)) continue;
		if (!SeenWidgets.Contains(UW))
		{
			ApplyIconSetToSingleWidget(UW);
			SeenWidgets.Add(UW);
		}
	}
	return true; // seguir tickeando
}
