#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Containers/Ticker.h"
#include "ControllerIconManagerSubsystem.generated.h"

UENUM(BlueprintType)
enum class EIconSet : uint8
{
	PC,
	PS5_UI,
	XBOX_UI
};

UCLASS()
class DARKLIFEEXCALIBUR_API UControllerIconManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Reescaneo manual (por si creas widgets después de un cambio). */
	UFUNCTION(BlueprintCallable, Category = "ControllerIcons", meta = (WorldContext = "WorldContextObject"))
	void ForceRescan(UObject* WorldContextObject);

private:
	/** Sólo estos nombres de Texture2D se sustituyen. */
	TSet<FName> AllowedNames;

	/** Ruta base (assets de Unreal, no del disco). */
	FString BaseRoot = TEXT("/Game/Assets2D/icon_PS4_PS5");

	/** Variantes de carpeta por set (acepta may/min). */
	TMap<EIconSet, TArray<FString>> FolderNamesPerSet;

	/** Set actual detectado. */
	EIconSet CurrentSet = EIconSet::PC;

	/** Re-escaneo tras cargar mapa. */
	FDelegateHandle PostLoadMapHandle;

	/** Polling (robusto en Shipping). */
	FTSTicker::FDelegateHandle PollTickerHandle;
	bool TickPoll(float DeltaTime);
	bool bLastPolledGamepadAttached = false;

	/** Escaneo periódico de widgets NUEVOS (para tutoriales/overlays tardíos). */
	FTSTicker::FDelegateHandle NewWidgetScanTickerHandle;
	bool TickScanNewWidgets(float DeltaTime);

	/** Conjunto de widgets ya procesados (evita reprocesar los mismos cada tick). */
	TSet<TWeakObjectPtr<class UUserWidget>> SeenWidgets;

	/** UE 5.2: delegado de conexión (útil en Shipping). */
#if (ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION <= 2)
	FDelegateHandle ControllerConnHandle;
	TSet<int32> ActiveControllers;
#endif

	/** Caché y keep-alive de texturas cargadas (evita GC en Shipping). */
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<class UTexture2D>> LoadedCache;

	// Preferencia y arranque
	bool HasAnyGamepadAttached() const;
	void EvaluateInitialDevice();

	// Detección / aplicación
	void BindInputDetection();
	void UnbindInputDetection();
	void OnPostLoadMap(UWorld* LoadedWorld);

	void OnInputChangedToKeyboard();
	void OnInputChangedToGamepadGeneric();
	void OnGamepadTypePlayStation();
	void OnGamepadTypeXbox();

	void SetIconSet(EIconSet NewSet);
	void ClearCache();

	void ApplyIconSetToAllWidgets(UWorld* World);
	void ApplyIconSetToSingleWidget(class UUserWidget* RootWidget);
	void MaybeReplaceBrush(struct FSlateBrush& Brush);

	// Resolución de textura variante
	class UTexture2D* ResolveVariantTextureFromOld(class UTexture2D* OldTex);
	UObject* TryLoadVariantObject(const FString& AssetName);
	const TArray<FString>& GetFolderCandidates() const;

	bool IsAllowedName(const FString& AssetName) const;

	// -------- NUEVO: helper de plataforma --------
	static bool IsPlayStationPlatform()
	{
#if PLATFORM_PS5 || PLATFORM_PS4
		return true;
#else
		return false;
#endif
	}
};
