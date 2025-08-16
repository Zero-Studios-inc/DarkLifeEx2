#include "DLE_EOSSubsystem.h"

void ShowScreen(const FString& Msg, FColor Color = FColor::Cyan, float Time = 4.f)
{
    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, Time, Color, Msg);
}

void UDLE_EOSSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    ShowScreen(TEXT("[EOSSub] Init"), FColor::Green);

    // Usará EOS por tu DefaultEngine.ini → [OnlineSubsystem] DefaultPlatformService=EOS
    OSS = IOnlineSubsystem::Get();
    if (!OSS)
    {
        UE_LOG(LogTemp, Error, TEXT("[EOSSub] IOnlineSubsystem::Get() == nullptr. Revisa plugins/ini."));
        ShowScreen(TEXT("[EOSSub] OSS NULL (revisa Plugins/.ini)"), FColor::Red);
        return;
    }

    Identity = OSS->GetIdentityInterface();
    Achievements = OSS->GetAchievementsInterface();

    if (!Identity.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("[EOSSub] IdentityInterface no disponible."));
        ShowScreen(TEXT("[EOSSub] Identity NULL (plugin OSS?)"), FColor::Red);
        return;
    }
    if (!Achievements.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[EOSSub] AchievementsInterface no disponible (plugin OSS/EOS?)."));
        ShowScreen(TEXT("[EOSSub] Achievements NULL (plugin OSS/EOS?)"), FColor::Yellow);
        // seguimos; el login puede funcionar igual y ya avisará al escribir logro
    }

    // Escuchar resultado de login
    LoginCompleteHandle = Identity->AddOnLoginCompleteDelegate_Handle(
        LocalUserNum,
        FOnLoginCompleteDelegate::CreateUObject(this, &UDLE_EOSSubsystem::OnLoginComplete)
    );

    LoginWithDeveloper(); // auto-login al iniciar
}

void UDLE_EOSSubsystem::Deinitialize()
{
    if (Identity.IsValid() && LoginCompleteHandle.IsValid())
    {
        Identity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, LoginCompleteHandle);
    }
    Super::Deinitialize();
}

void UDLE_EOSSubsystem::LoginWithDeveloper()
{
    if (!Identity.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("[EOSSub] IdentityInterface no válido."));
        ShowScreen(TEXT("[EOSSub] Identity INVALID"), FColor::Red);
        return;
    }

    // DevAuth Tool: localhost:6300, Token: DarkLifeExZS
    FOnlineAccountCredentials Creds;
    Creds.Type = TEXT("developer");
    Creds.Id = TEXT("127.0.0.1:6300");
    Creds.Token = TEXT("DarkLifeExZS");

    UE_LOG(LogTemp, Warning, TEXT("[EOSSub] Iniciando login dev a %s con token %s"),
        *Creds.Id, *Creds.Token);
    ShowScreen(TEXT("[EOSSub] Login dev → 127.0.0.1:6300 / DarkLifeExZS"), FColor::Cyan);

    const bool bStarted = Identity->Login(LocalUserNum, Creds);
    if (!bStarted)
    {
        UE_LOG(LogTemp, Error, TEXT("[EOSSub] Identity->Login no pudo iniciar."));
        ShowScreen(TEXT("[EOSSub] Identity->Login no pudo iniciar"), FColor::Red);
    }
}

void UDLE_EOSSubsystem::ForceLogin()
{
    bLoggedIn = false;
    ShowScreen(TEXT("[EOSSub] ForceLogin"), FColor::Cyan);
    LoginWithDeveloper();
}

void UDLE_EOSSubsystem::OnLoginComplete(int32 InLocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
    if (!bWasSuccessful)
    {
        UE_LOG(LogTemp, Error, TEXT("[EOSSub] Login falló: %s"), *Error);
        ShowScreen(FString::Printf(TEXT("[EOSSub] Login FAIL: %s"), *Error), FColor::Red, 6.f);
        return;
    }

    bLoggedIn = true;
    LocalUserNum = InLocalUserNum;
    CachedUserId = UserId.AsShared();

    UE_LOG(LogTemp, Log, TEXT("[EOSSub] Login OK. UserId: %s"), *UserId.ToString());
    ShowScreen(TEXT("[EOSSub] Login OK"), FColor::Green, 5.f);

    if (Achievements.IsValid() && CachedUserId.IsValid())
    {
        Achievements->QueryAchievements(
            *CachedUserId.Get(),
            FOnQueryAchievementsCompleteDelegate::CreateUObject(this, &UDLE_EOSSubsystem::OnQueryAchievementsComplete)
        );
    }
}

void UDLE_EOSSubsystem::OnQueryAchievementsComplete(const FUniqueNetId& UserId, const bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("[EOSSub] QueryAchievements %s"), bWasSuccessful ? TEXT("OK") : TEXT("FAIL"));
    ShowScreen(FString::Printf(TEXT("[EOSSub] QueryAchievements %s"), bWasSuccessful ? TEXT("OK") : TEXT("FAIL")));
}

void UDLE_EOSSubsystem::UnlockAchievementByApiName(const FString& AchievementId)
{
    if (!bLoggedIn || !CachedUserId.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[EOSSub] Aún no logueado; no se puede desbloquear %s"), *AchievementId);
        ShowScreen(TEXT("[EOSSub] No logueado; no se puede desbloquear"), FColor::Red);
        return;
    }
    if (!Achievements.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("[EOSSub] AchievementsInterface no disponible."));
        ShowScreen(TEXT("[EOSSub] Achievements NULL"), FColor::Red);
        return;
    }

    // UE 5.2 (OSS clásico): TMap<FName, FVariantData>
    FOnlineAchievementsWriteRef WriteObject = MakeShareable(new FOnlineAchievementsWrite());

    FVariantData Progress;
    Progress.SetValue(100.0f); // 0..100
    WriteObject->Properties.Add(FName(*AchievementId), Progress);

    ShowScreen(FString::Printf(TEXT("[EOSSub] Intentando desbloquear: %s"), *AchievementId), FColor::Yellow);

    Achievements->WriteAchievements(
        *CachedUserId.Get(),
        WriteObject,
        FOnAchievementsWrittenDelegate::CreateLambda(
            [AchievementId](const FUniqueNetId& UserId, bool bSuccess)
            {
                if (bSuccess)
                {
                    UE_LOG(LogTemp, Log, TEXT("[EOSSub] Logro desbloqueado: %s"), *AchievementId);
                    ShowScreen(FString::Printf(TEXT("[EOSSub] Logro OK: %s"), *AchievementId), FColor::Green);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("[EOSSub] Fallo al desbloquear: %s"), *AchievementId);
                    ShowScreen(FString::Printf(TEXT("[EOSSub] Logro FAIL: %s"), *AchievementId), FColor::Red);
                }
            }
        )
    );
}
