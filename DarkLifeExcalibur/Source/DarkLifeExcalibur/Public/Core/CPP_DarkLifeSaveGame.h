// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CPP_DarkLifeSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class DARKLIFEEXCALIBUR_API UCPP_DarkLifeSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY( BlueprintReadWrite)
		int HealAmount;
	UPROPERTY(BlueprintReadWrite)
		int HealBackup;
	UPROPERTY(BlueprintReadWrite)
		double CurrentHealth;
	UPROPERTY(BlueprintReadWrite)
		double CurrentRecharge;
	UPROPERTY(BlueprintReadWrite)
		double CurrentStamina;
	UPROPERTY(BlueprintReadWrite)
		double CurrentFracture;
	UPROPERTY(BlueprintReadWrite)
		double CurrentBeast;
	UPROPERTY(BlueprintReadWrite)
		double CurrentDefense;
	UPROPERTY(BlueprintReadWrite)
		int32 SelectedShieldIndex;
	UPROPERTY(BlueprintReadWrite)
		FName StreamingLevelName;

	//Character
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		int32 CharacterLevel;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		double CharacterXP;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		double CharacterHealthBase;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		double CharacterRechargeBase;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		double CharacterBeastBase;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		double CharacterStaminaBase;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		double CharacterDefenseBase;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		double CharacterFractureBase;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		FTransform LastSavePoint;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		double CharacterXPBase;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		int32 ProjectileAmount;



	
};
