// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Core/CPP_DarkLifeSaveGame.h"
#include "CPP_GameInstance.generated.h"

class ACPP_DarkLifeCharacter;
class UCPP_DarkLifeSaveGame;

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

	

public:
	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void SaveGame();
	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void LoadGame();		

};
