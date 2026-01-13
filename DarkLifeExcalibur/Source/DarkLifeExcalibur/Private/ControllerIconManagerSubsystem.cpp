#include "ControllerIconManagerSubsystem.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Styling/SlateBrush.h"
#include "Engine/World.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/WeakObjectPtr.h"
#include "Misc/CoreDelegates.h"
#include "Framework/Application/SlateApplication.h"
#include "InputCoreTypes.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
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

static void LogIconSet(const TCHAR* Where, EIconSet Set)
{
	UE_LOG(LogTemp, Log, TEXT("[ControllerIcons][%s] IconSet=%d (0=PC,1=PS5_UI,2=XBOX_UI)"), Where, (int32)Set);
}

void UControllerIconManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AllowedNames = {
		FName("Bumper_R"),
		FName("Trigger_R"),
		FName("Trigger_L"),
		FName("FaceRight"),
		FName("FaceUp"),
		FName("FaceLeft"),
		FName("DPad_Up"),
		FName("DPad_Down"),
		FName("Bumper_L"),
		FName("Bumper_R2")
	};

	FolderNamesPerSet.Add(EIconSet::PS5_UI, { TEXT("PS5_UI"),  TEXT("ps5_ui") });
	FolderNamesPerSet.Add(EIconSet::XBOX_UI, { TEXT("XBOX_UI"), TEXT("xbox_ui") });
	FolderNamesPerSet.Add(EIconSet::PC, { TEXT("Pc"),      TEXT("pc"), TEXT("PC") });

	// Set inicial por plataforma (NO aplicar aún; World puede ser null aquí)
	if (IsPlayStationPlatform()) SetIconSet(EIconSet::PS5_UI);
	else                         SetIconSet(EIconSet::PC);

	// Re-escaneo al cargar mapa
	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(
		this, &UControllerIconManagerSubsystem::OnPostLoadMap);

	BindInputDetection();

	// Forzar por CLI: -forceiconset=pc|xbox|xbox_ui|ps5|ps5_ui
	{
		FString Force;
		if (FParse::Value(FCommandLine::Get(), TEXT("forceiconset="), Force))
		{
			Force = Force.ToLower();
			if (Force == TEXT("pc")) SetIconSet(EIconSet::PC);
			else if (Force == TEXT("xbox") || Force == TEXT("xbox_ui")) SetIconSet(EIconSet::XBOX_UI);
			else if (Force == TEXT("ps5") || Force == TEXT("ps5_ui"))  SetIconSet(EIconSet::PS5_UI);
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

	// Warmup y aplicación diferida (evita pegarle a UMG antes de que el mundo esté listo)
	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateWeakLambda(this, [this](float)
			{
				if (UWorld* World = GetWorld())
				{
					WarmupCurrentSet();
					ApplyIconSetToAllWidgets(World);
					StartNewWidgetScanWindow(NewWidgetScanWindowSeconds);
					LogIconSet(TEXT("InitApply"), CurrentSet);
				}
				return false;
			}),
		0.25f
	);
}

void UControllerIconManagerSubsystem::Deinitialize()
{
	UnbindInputDetection();

	StopNewWidgetScanTicker();
	SeenWidgets.Empty();
	ClearCache();

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
				CIS->OnInputMethodChangedNative.AddWeakLambda(this, [this](ECommonInputType NewType)
					{
						if (NewType == ECommonInputType::MouseAndKeyboard) OnInputChangedToKeyboard();
						else OnInputChangedToGamepadGeneric();
					});

				// Tipo de gamepad
				CIS->OnGamepadInputTypeChangedNative.AddWeakLambda(this, [this](ECommonGamepadType T)
					{
						if (T == ECommonGamepadType::Playstation) OnGamepadTypePlayStation();
						else if (T == ECommonGamepadType::Xbox)  OnGamepadTypeXbox();
						else OnInputChangedToGamepadGeneric();
					});

				// Estado inicial
				if (CIS->GetCurrentInputType() == ECommonInputType::MouseAndKeyboard) OnInputChangedToKeyboard();
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

		// Polling robusto (Shipping)
		PollTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
			FTickerDelegate::CreateUObject(this, &UControllerIconManagerSubsystem::TickPoll),
			0.25f
		);

	const bool bAttached = FSlateApplication::IsInitialized() && FSlateApplication::Get().IsGamepadAttached();
	bLastPolledGamepadAttached = bAttached;

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
	// Hot-plug
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

		// Solo log cuando cambia
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
		EKeys::Gamepad_LeftTrigger,       EKeys::Gamepad_RightTrigger
	};

	static const TArray<FKey> KBMKeys = {
		EKeys::LeftMouseButton, EKeys::RightMouseButton,
		EKeys::W, EKeys::A, EKeys::S, EKeys::D,
		EKeys::SpaceBar, EKeys::Enter, EKeys::Escape
	};

	if (Local_AnyKeyDown(PC, GamepadKeys))
	{
		if (IsPlayStationPlatform()) OnGamepadTypePlayStation();
		else                         OnInputChangedToGamepadGeneric();
	}
	else if (!bAttachedNow && Local_AnyKeyDown(PC, KBMKeys))
	{
		OnInputChangedToKeyboard();
	}

	return true;
}

void UControllerIconManagerSubsystem::OnPostLoadMap(UWorld* LoadedWorld)
{
	if (!LoadedWorld) return;

	// Reset “seen” por mapa nuevo
	SeenWidgets.Empty();

	// Aplicación y ventana de scan (solo por unos segundos)
	WarmupCurrentSet();
	ApplyIconSetToAllWidgets(LoadedWorld);
	StartNewWidgetScanWindow(NewWidgetScanWindowSeconds);

	// Re-escaneo corto diferido, pero SAFE: no capturar UWorld* crudo
	TWeakObjectPtr<UWorld> WeakWorld = LoadedWorld;

	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateWeakLambda(this, [this, WeakWorld](float)
			{
				if (!WeakWorld.IsValid()) return false;

				ApplyIconSetToAllWidgets(WeakWorld.Get());
				EvaluateInitialDevice();
				return false;
			}),
		0.35f
	);
}

void UControllerIconManagerSubsystem::OnInputChangedToKeyboard()
{
	if (HasAnyGamepadAttached()) return;
	SetIconSet(EIconSet::PC);
	if (UWorld* World = GetWorld())
	{
		ApplyIconSetToAllWidgets(World);
		StartNewWidgetScanWindow(NewWidgetScanWindowSeconds);
	}
}

void UControllerIconManagerSubsystem::OnInputChangedToGamepadGeneric()
{
	if (IsPlayStationPlatform()) SetIconSet(EIconSet::PS5_UI);
	else                         SetIconSet(EIconSet::XBOX_UI);

	if (UWorld* World = GetWorld())
	{
		ApplyIconSetToAllWidgets(World);
		StartNewWidgetScanWindow(NewWidgetScanWindowSeconds);
	}
}

void UControllerIconManagerSubsystem::OnGamepadTypePlayStation()
{
	SetIconSet(EIconSet::PS5_UI);
	if (UWorld* World = GetWorld())
	{
		ApplyIconSetToAllWidgets(World);
		StartNewWidgetScanWindow(NewWidgetScanWindowSeconds);
	}
}

void UControllerIconManagerSubsystem::OnGamepadTypeXbox()
{
	SetIconSet(EIconSet::XBOX_UI);
	if (UWorld* World = GetWorld())
	{
		ApplyIconSetToAllWidgets(World);
		StartNewWidgetScanWindow(NewWidgetScanWindowSeconds);
	}
}

void UControllerIconManagerSubsystem::SetIconSet(EIconSet NewSet)
{
	if (CurrentSet == NewSet) return;

	CurrentSet = NewSet;
	ClearCache();
	SeenWidgets.Empty();

	// Warmup cuando cambias set (evita stutter al primer widget)
	WarmupCurrentSet();
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
		SeenWidgets.Empty();
		WarmupCurrentSet();
		ApplyIconSetToAllWidgets(World);
		StartNewWidgetScanWindow(NewWidgetScanWindowSeconds);
	}
}

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

void UControllerIconManagerSubsystem::ApplyIconSetToSingleWidget(UUserWidget* UW)
{
	if (!IsValid(UW) || !UW->WidgetTree) return;

	TArray<UWidget*> Widgets;
	UW->WidgetTree->GetAllWidgets(Widgets);

	for (UWidget* W : Widgets)
	{
		// FIX: en consola/Shipping a veces hay widgets en teardown
		if (!IsValid(W)) continue;

		if (UImage* Img = Cast<UImage>(W))
		{
			UTexture2D* OldTex = Cast<UTexture2D>(Img->GetBrush().GetResourceObject());
			if (!IsValid(OldTex)) continue;
			if (!IsAllowedName(OldTex->GetName())) continue;

			if (UTexture2D* NewTex = ResolveVariantTextureFromOld(OldTex))
			{
				Img->SetBrushFromTexture(NewTex, true);
			}
		}
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
	// FIX CRASH (PS5): NO usar operator[] en TMap aquí
	if (const TArray<FString>* Found = FolderNamesPerSet.Find(CurrentSet))
	{
		return *Found;
	}

	// Fallback seguro
	static const TArray<FString> Fallback = { TEXT("PC"), TEXT("pc"), TEXT("Pc") };
	UE_LOG(LogTemp, Error, TEXT("[ControllerIcons] Missing FolderNamesPerSet for CurrentSet=%d. Using fallback."), (int32)CurrentSet);
	return Fallback;
}

UObject* UControllerIconManagerSubsystem::TryLoadVariantObject(const FString& AssetName)
{
	// Cache hit
	if (const TObjectPtr<UTexture2D>* Found = LoadedCache.Find(FName(*AssetName)))
	{
		return Found->Get();
	}

	const TArray<FString>& Folders = GetFolderCandidates();

	for (const FString& Folder : Folders)
	{
		const FString ObjectPath = FString::Printf(
			TEXT("%s/%s/%s.%s"), *BaseRoot, *Folder, *AssetName, *AssetName);

		const FSoftObjectPath SoftPath(ObjectPath);
		if (!SoftPath.IsValid()) continue;

		FStreamableManager& SM = UAssetManager::GetStreamableManager();
		if (UObject* Obj = SM.LoadSynchronous(SoftPath, false))
		{
			if (UTexture2D* AsTex = Cast<UTexture2D>(Obj))
			{
				LoadedCache.Add(FName(*AssetName), TObjectPtr<UTexture2D>(AsTex));
				return AsTex;
			}
			return Obj;
		}
	}

	// OJO: no log aquí en shipping cada vez, porque esto puede spamear y pegar rendimiento
	return nullptr;
}

UTexture2D* UControllerIconManagerSubsystem::ResolveVariantTextureFromOld(UTexture2D* OldTex)
{
	if (!IsValid(OldTex)) return nullptr;

	if (UObject* Obj = TryLoadVariantObject(OldTex->GetName()))
	{
		return Cast<UTexture2D>(Obj);
	}
	return nullptr;
}

bool UControllerIconManagerSubsystem::IsAllowedName(const FString& AssetName) const
{
	return AllowedNames.Contains(FName(*AssetName));
}

void UControllerIconManagerSubsystem::StartNewWidgetScanWindow(float Seconds)
{
	NewWidgetScanRemaining = FMath::Max(Seconds, 0.25f);

	// Si no existe ticker, lo creamos
	if (!NewWidgetScanTickerHandle.IsValid())
	{
		NewWidgetScanTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
			FTickerDelegate::CreateUObject(this, &UControllerIconManagerSubsystem::TickScanNewWidgets),
			0.2f // más rápido que 0.5s pero solo corre poco tiempo
		);
	}
}

void UControllerIconManagerSubsystem::StopNewWidgetScanTicker()
{
	if (NewWidgetScanTickerHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(NewWidgetScanTickerHandle);
		NewWidgetScanTickerHandle.Reset();
	}
	NewWidgetScanRemaining = 0.0f;
}

bool UControllerIconManagerSubsystem::TickScanNewWidgets(float DeltaTime)
{
	NewWidgetScanRemaining -= DeltaTime;
	if (NewWidgetScanRemaining <= 0.0f)
	{
		StopNewWidgetScanTicker();
		return false;
	}

	UWorld* World = GetWorld();
	if (!World) return true;

	TArray<UUserWidget*> AllWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(World, AllWidgets, UUserWidget::StaticClass(), false);

	for (UUserWidget* UW : AllWidgets)
	{
		if (!IsValid(UW)) continue;

		// evitar reprocesar
		if (!SeenWidgets.Contains(UW))
		{
			ApplyIconSetToSingleWidget(UW);
			SeenWidgets.Add(UW);
		}
	}
	return true;
}

void UControllerIconManagerSubsystem::WarmupCurrentSet()
{
	// Precarga ligera: intenta resolver los AllowedNames una sola vez
	// Esto reduce los picos cuando aparece el primer HUD.
	for (const FName& N : AllowedNames)
	{
		const FString NameStr = N.ToString();
		(void)TryLoadVariantObject(NameStr);
	}
}
