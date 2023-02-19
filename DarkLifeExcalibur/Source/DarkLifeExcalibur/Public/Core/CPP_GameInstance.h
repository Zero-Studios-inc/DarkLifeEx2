// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CPP_DarkLifeCharacter.h"
#include "CPP_DarkLifeSaveGame.h"
#include "CPP_GameInstance.generated.h"

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
		bool bNewGame;
	UPROPERTY(BlueprintReadWrite)
		int32 SavePointAnimationIndex;
	UPROPERTY(BlueprintReadWrite)
		bool bCleanStart;
	UPROPERTY(BlueprintReadOnly)
		ACPP_DarkLifeCharacter* CharacterRef;
	
};
