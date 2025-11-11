// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Core/CPP_DarkLifeSaveGame.h"
#include "CPP_GameInstance.generated.h"

class ACPP_DarkLifeCharacter;
class UCPP_DarkLifeSaveGame;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRuneCooldownFinishedEvent, int, RuneSlot);
/**
 * 
 */
UCLASS()
class DARKLIFEEXCALIBUR_API UCPP_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		FString SlotName = "DLSaveGame";
	UPROPERTY(BlueprintReadWrite)
		bool bNewGame = true;
	UPROPERTY(BlueprintReadWrite)
		int32 SavePointAnimationIndex;
	UPROPERTY(BlueprintReadWrite)
		bool bCleanStart;
	UPROPERTY(BlueprintReadOnly)
		ACPP_DarkLifeCharacter* CharacterRef;
    UPROPERTY(BlueprintReadWrite)
        UCPP_DarkLifeSaveGame* SaveGameObject;
	UPROPERTY(BlueprintReadWrite)
	bool bGameLoaded = false;
	UPROPERTY(BlueprintReadWrite)
	TMap<UCPP_DA_Item_Rune*, FTimerHandle> RunesCooldownRegistry;

	UPROPERTY(BlueprintAssignable, Category = "Runes|Cooldown")
	FOnRuneCooldownFinishedEvent OnRuneCooldownFinishedEvent;
	

public:
	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void SaveGame();
	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void LoadGame();		
    UFUNCTION(BlueprintCallable, Category = "Runes")
	void RunesUseCoolDown(UCPP_DA_Item_Rune* Rune, int RuneSlot);
	UFUNCTION(BlueprintCallable, Category = "Runes")
	void OnRuneCooldownFinished(int RuneSlot);
	UFUNCTION(BlueprintCallable, Category = "Runes")
	bool IsRuneOnCooldown(int RuneSlot) const;
	UFUNCTION(BlueprintCallable, Category = "Runes|Cooldown")
	float GetRemainingCooldownTime(int RuneSlot) const;
	UFUNCTION(BlueprintCallable, Category = "Runes|Cooldown")
	float GetRuneRemainingTime(int RuneSlot);

private:
	UPROPERTY()
	FTimerHandle RuneSlot00CoolDownTimer;
	UPROPERTY()
	FTimerHandle RuneSlot01CoolDownTimer;
	UPROPERTY()
	FTimerHandle RuneSlot02CoolDownTimer;
	UPROPERTY()
	FTimerHandle RuneSlot03CoolDownTimer;
};
