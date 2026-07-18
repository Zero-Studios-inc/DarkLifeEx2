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
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerController.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "GameLoadLog.h"

#if __has_include("CommonInputSubsystem.h")
#include "CommonInputSubsystem.h"
#define HAS_COMMONUI 1
#else
#define HAS_COMMONUI 0
#endif

// Log auxiliar
static void LogIconSet(const TCHAR* Where, EIconSet Set)
{
	UE_LOG(LogTemp, Log, TEXT("[ControllerIcons][%s] IconSet=%d (0=PC,1=PS5_UI,2=XBOX_UI)"), Where, (int32)Set);
}

void UControllerIconManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Nombres a sustituir (los que usa tu HUD)
	AllowedNames = {
		FName("Bumper_R"),   // R1 / RB
		FName("Trigger_R"),  // R2 / RT
		FName("Trigger_L"),  // L2 / LT
		FName("FaceRight"),  // ○ / B
		FName("FaceUp"),     // △ / Y
		FName("FaceLeft"),   // □ / X
		FName("DPad_Up"),
		FName("DPad_Down"),
		FName("Bumper_L"),
		FName("Bumper_R2")
	};

	// Carpetas por plataforma (tolerante a may/min)
	FolderNamesPerSet.Add(EIconSet::PS5_UI, { TEXT("PS5_UI"),  TEXT("ps5_ui") });
	FolderNamesPerSet.Add(EIconSet::XBOX_UI, { TEXT("XBOX_UI"), TEXT("xbox_ui") });
	FolderNamesPerSet.Add(EIconSet::PC, { TEXT("Pc"),      TEXT("pc"), TEXT("PC") });

	// -------- CAMBIO: set inicial por plataforma --------
	if (IsPlayStationPlatform())
	{
		SetIconSet(EIconSet::PS5_UI);
	}
	else
	{
		SetIconSet(EIconSet::PC);
	}

	// Re-escaneo al cargar mapa
	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(
		this, &UControllerIconManagerSubsystem::OnPostLoadMap);

	BindInputDetection();

	// Permite forzar por CLI (Shipping): -forceiconset=pc|xbox|xbox_ui|ps5|ps5_ui
	{
		FString Force;
		if (FParse::Value(FCommandLine::Get(), TEXT("forceiconset="), Force))
		{
			Force = Force.ToLower();
			if (Force == TEXT("pc"))
			{
				SetIconSet(EIconSet::PC);
			}
			else if (Force == TEXT("xbox") || Force == TEXT("xbox_ui"))
			{
				SetIconSet(EIconSet::XBOX_UI);
			}
			else if (Force == TEXT("ps5") || Force == TEXT("ps5_ui"))
			{
				SetIconSet(EIconSet::PS5_UI);
			}
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

	// -------- CAMBIO: en PS5, si hay mando adjunto, usar PlayStation por defecto --------
	if (bAttached)
	{
		if (IsPlayStationPlatform()) OnGamepadTypePlayStation();
		else                         OnInputChangedToGamepadGeneric();
	}
	else
	{
		OnInputChangedToKeyboard();
	}
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
					if (IsPlayStationPlatform()) { OnGamepadTypePlayStation(); return; }

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
					if (IsPlayStationPlatform()) { OnGamepadTypePlayStation(); return; }

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

	// Sin CommonUI / no disponible
	if (HasAnyGamepadAttached())
	{
		if (IsPlayStationPlatform()) OnGamepadTypePlayStation();
		else                         OnInputChangedToGamepadGeneric();
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
		if (bAttachedNow)
		{
			if (IsPlayStationPlatform()) OnGamepadTypePlayStation();
			else                         OnInputChangedToGamepadGeneric();
		}
		else
		{
			OnInputChangedToKeyboard();
		}
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
		if (IsPlayStationPlatform()) OnGamepadTypePlayStation();
		else                         OnInputChangedToGamepadGeneric();
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
	if (!ensureMsgf(IsValid(LoadedWorld), TEXT("Controller icon rescan received an invalid loaded world")))
	{
		return;
	}

	UE_LOG(LogGameLoad, Log, TEXT("Level ready for UI rescan: World=%s Thread=%s"),
		*GetNameSafe(LoadedWorld), IsInGameThread() ? TEXT("GameThread") : TEXT("OtherThread"));
	ApplyIconSetToAllWidgets(LoadedWorld);

	// The GameInstance subsystem survives map changes. Never retain a raw UWorld pointer
	// in these delayed callbacks because the referenced map may unload before they run.
	const TWeakObjectPtr<UWorld> WeakLoadedWorld(LoadedWorld);
	const auto ScheduleRescan = [this, WeakLoadedWorld](float Delay)
	{
		FTSTicker::GetCoreTicker().AddTicker(
			FTickerDelegate::CreateWeakLambda(this, [this, WeakLoadedWorld, Delay](float)
			{
				UWorld* World = WeakLoadedWorld.Get();
				if (!IsValid(World) || World != GetWorld())
				{
					UE_LOG(LogGameLoad, Warning,
						TEXT("Deferred UI rescan skipped: Delay=%.2f World=%s CurrentWorld=%s"),
						Delay, *GetNameSafe(World), *GetNameSafe(GetWorld()));
					return false;
				}

				ApplyIconSetToAllWidgets(World);
				EvaluateInitialDevice();
				UE_LOG(LogGameLoad, Verbose, TEXT("Deferred UI rescan completed: Delay=%.2f World=%s"),
					Delay, *GetNameSafe(World));
				return false;
			}), Delay);
	};

	ScheduleRescan(0.25f);
	ScheduleRescan(1.0f);
	ScheduleRescan(2.5f);
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
	// -------- CAMBIO: en PS5, el "genérico" debe ser PlayStation --------
	if (IsPlayStationPlatform())
	{
		SetIconSet(EIconSet::PS5_UI);
	}
	else
	{
		SetIconSet(EIconSet::XBOX_UI);
	}
	if (UWorld* World = GetWorld()) ApplyIconSetToAllWidgets(World);
}

void UControllerIconManagerSubsystem::OnGamepadTypePlayStation()
{
	SetIconSet(EIconSet::PS5_UI);
	if (UWorld* World = GetWorld()) ApplyIconSetToAllWidgets(World);
}

void UControllerIconManagerSubsystem::OnGamepadTypeXbox()
{
	SetIconSet(EIconSet::XBOX_UI);
	if (UWorld* World = GetWorld()) ApplyIconSetToAllWidgets(World);
}

void UControllerIconManagerSubsystem::SetIconSet(EIconSet NewSet)
{
	if (CurrentSet == NewSet) return;
	CurrentSet = NewSet;
	ClearCache();          // limpiar caché al cambiar familia (PS5_UI/XBOX_UI/PC)
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
	if (!IsValid(World)) return;

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
