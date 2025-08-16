#include "DLE_OnlineGameInstance.h"

#include "OnlineSubsystemTypes.h" // FVariantData, etc.

void UDLE_OnlineGameInstance::Init()
{
    Super::Init();

    // Usará EOS por tu DefaultEngine.ini → [OnlineSubsystem] DefaultPlatformService=EOS
    OSS = IOnlineSubsystem::Get();
    if (!OSS)
    {
        UE_LOG(LogTemp, Error, TEXT("IOnlineSubsystem::Get() == nullptr. Revisa plugins/ini."));
        return;
    }

    Identity = OSS->GetIdentityInterface();
    Achievements = OSS->GetAchievementsInterface();

    if (!Identity.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("IdentityInterface no disponible."));
        return;
    }
    if (!Achievements.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("AchievementsInterface no disponible (plugin OSS/EOS?)."));
    }

    Identity->AddOnLoginCompleteDelegate_Handle(
        LocalUserNum,
        FOnLoginCompleteDelegate::CreateUObject(this, &UDLE_OnlineGameInstance::OnLoginComplete)
    );

    LoginWithDeveloper();
}

void UDLE_OnlineGameInstance::LoginWithDeveloper()
{
    if (!Identity.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("IdentityInterface no válido."));
        return;
    }

    // DevAuth Tool: localhost:6300, Token: DarkLifeExZS
    FOnlineAccountCredentials Creds;
    Creds.Type = TEXT("developer");
    Creds.Id = TEXT("127.0.0.1:6300");
    Creds.Token = TEXT("DarkLifeExZS");

    UE_LOG(LogTemp, Warning, TEXT("[DLE] Iniciando login dev a %s con token %s"),
        *Creds.Id, *Creds.Token);

    const bool bStarted = Identity->Login(LocalUserNum, Creds);
    if (!bStarted)
    {
        UE_LOG(LogTemp, Error, TEXT("Identity->Login no pudo iniciar."));
    }
}

void UDLE_OnlineGameInstance::OnLoginComplete(int32 InLocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
    if (!bWasSuccessful)
    {
        UE_LOG(LogTemp, Error, TEXT("Login falló: %s"), *Error);
        bLoggedIn = false;
        return;
    }

    bLoggedIn = true;
    LocalUserNum = InLocalUserNum;
    CachedUserId = UserId.AsShared();

    UE_LOG(LogTemp, Log, TEXT("Login OK. UserId: %s"), *UserId.ToString());

    if (Achievements.IsValid() && CachedUserId.IsValid())
    {
        Achievements->QueryAchievements(
            *CachedUserId.Get(),
            FOnQueryAchievementsCompleteDelegate::CreateUObject(this, &UDLE_OnlineGameInstance::OnQueryAchievementsComplete)
        );
    }
}

void UDLE_OnlineGameInstance::OnQueryAchievementsComplete(const FUniqueNetId& UserId, const bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("QueryAchievements %s"), bWasSuccessful ? TEXT("OK") : TEXT("FAIL"));
}

void UDLE_OnlineGameInstance::UnlockAchievementByApiName(const FString& AchievementId)
{
    if (!bLoggedIn || !CachedUserId.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Aún no logueado; no se puede desbloquear %s"), *AchievementId);
        return;
    }
    if (!Achievements.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("AchievementsInterface no disponible."));
        return;
    }

    // UE 5.2 (OSS clásico): TMap<FName, FVariantData>
    FOnlineAchievementsWriteRef WriteObject = MakeShareable(new FOnlineAchievementsWrite());

    FVariantData Progress;
    Progress.SetValue(100.0f); // 0..100
    WriteObject->Properties.Add(FName(*AchievementId), Progress);

    Achievements->WriteAchievements(
        *CachedUserId.Get(),
        WriteObject,
        FOnAchievementsWrittenDelegate::CreateLambda(
            [AchievementId](const FUniqueNetId& UserId, bool bSuccess)
            {
                if (bSuccess)
                {
                    UE_LOG(LogTemp, Log, TEXT("Logro desbloqueado: %s"), *AchievementId);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Fallo al desbloquear: %s"), *AchievementId);
                }
            }
        )
    );
}

/** ===================== Comandos de consola ===================== */
bool UDLE_OnlineGameInstance::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
    // DLE_LoginStatus
    if (FParse::Command(&Cmd, TEXT("DLE_LoginStatus")))
    {
        const FString Status = bLoggedIn ? TEXT("LOGGED IN") : TEXT("NOT LOGGED IN");
        UE_LOG(LogTemp, Warning, TEXT("[DLE] LoginStatus: %s"), *Status);
        if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString::Printf(TEXT("[DLE] %s"), *Status)); }
        return true;
    }

    // DLE_UnlockCompleteGame
    if (FParse::Command(&Cmd, TEXT("DLE_UnlockCompleteGame")))
    {
        Unlock_CompleteGame();
        UE_LOG(LogTemp, Warning, TEXT("[DLE] Comando: DLE_UnlockCompleteGame"));
        if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("[DLE] unlock complete_game enviado")); }
        return true;
    }

    // DLE_ForceLogin
    if (FParse::Command(&Cmd, TEXT("DLE_ForceLogin")))
    {
        LoginWithDeveloper();
        UE_LOG(LogTemp, Warning, TEXT("[DLE] Comando: DLE_ForceLogin"));
        if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("[DLE] re-login enviado")); }
        return true;
    }

    return Super::Exec(InWorld, Cmd, Ar);
}
