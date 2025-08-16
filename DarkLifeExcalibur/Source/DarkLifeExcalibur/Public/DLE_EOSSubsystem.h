#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

// OSS clásico (UE 5.2)
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "OnlineSubsystemTypes.h"

#include "DLE_EOSSubsystem.generated.h"

/**
 * Subsystem que inicia sesión con DevAuth Tool y expone funciones para logros.
 * No requiere editar tu GameInstance.
 */
UCLASS(BlueprintType)  // <- Mejora A: ahora aparece en Blueprints (Get Game Instance Subsystem)
class DARKLIFEEXCALIBUR_API UDLE_EOSSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Se crea automáticamente al arrancar el juego
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /** Desbloquear por API Name (ej. "complete_game") */
    UFUNCTION(BlueprintCallable, Category = "EOS|Achievements")
    void UnlockAchievementByApiName(const FString& AchievementId);

    /** Helper para tu logro */
    UFUNCTION(BlueprintCallable, Category = "EOS|Achievements")
    void Unlock_CompleteGame() { UnlockAchievementByApiName(TEXT("complete_game")); }

    /** (Opcional) Reintentar login manualmente desde BP si hace falta */
    UFUNCTION(BlueprintCallable, Category = "EOS|Auth")
    void ForceLogin();

private:
    void LoginWithDeveloper();
    void OnLoginComplete(int32 InLocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
    void OnQueryAchievementsComplete(const FUniqueNetId& UserId, const bool bWasSuccessful);

private:
    IOnlineSubsystem* OSS = nullptr;
    IOnlineIdentityPtr Identity;
    IOnlineAchievementsPtr Achievements;

    int32 LocalUserNum = 0;
    TSharedPtr<const FUniqueNetId> CachedUserId;
    bool bLoggedIn = false;

    FDelegateHandle LoginCompleteHandle;
};
