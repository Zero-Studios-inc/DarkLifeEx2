#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

// UE 5.2 → Online Subsystem clásico
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineAchievementsInterface.h"

// Parse de comandos de consola
#include "Misc/Parse.h"

#include "DLE_OnlineGameInstance.generated.h"

// Macro API = <NombreModulo>_API → DarkLifeExcalibur_API
UCLASS()
class DARKLIFEEXCALIBUR_API UDLE_OnlineGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    // --- Atajos BP ---
    UFUNCTION(BlueprintCallable, Category = "EOS|Achievements")
    void UnlockAchievementByApiName(const FString& AchievementId);

    UFUNCTION(BlueprintCallable, Category = "EOS|Achievements")
    void Unlock_CompleteGame() { UnlockAchievementByApiName(TEXT("complete_game")); }

    // --- Comandos de consola ---
    // DLE_LoginStatus / DLE_UnlockCompleteGame / DLE_ForceLogin
    virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

private:
    // Login con DevAuth Tool
    void LoginWithDeveloper();

    // Callbacks OSS
    void OnLoginComplete(int32 InLocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
    void OnQueryAchievementsComplete(const FUniqueNetId& UserId, const bool bWasSuccessful);

private:
    IOnlineSubsystem* OSS = nullptr;
    IOnlineIdentityPtr Identity;
    IOnlineAchievementsPtr Achievements;

    int32 LocalUserNum = 0;
    TSharedPtr<const FUniqueNetId> CachedUserId;
    bool bLoggedIn = false;
};
