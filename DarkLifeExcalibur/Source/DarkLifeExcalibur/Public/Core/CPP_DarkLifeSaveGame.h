// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/Items/CPP_DA_Item.h"
#include "Data/Items/Runes/CPP_DA_Item_Rune_Attack.h"
#include "Data/Items/Runes/CPP_DA_Item_Rune_Pasive.h"
#include "Data/Items/CPP_DA_Item_ExcaliburPommel.h"
#include "Data/Items/CPP_DA_Item_ExcaliburGrip.h"
#include "Data/Items/CPP_DA_Item_ExcaliburCrossguard.h"
#include "Data/Items/CPP_DA_Item_ExcaliburBlade.h"
#include "../Data/Items/CPP_DA_Item_Shield.h"
#include "Containers/Map.h"
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
		int32 CharacterLevel = 1;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		double CharacterXP;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		double CharacterHealthBase = 100.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		double CharacterRechargeBase = 1.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		double CharacterBeastBase;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		double CharacterStaminaBase = 200.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		double CharacterDefenseBase = 10.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		double CharacterFractureBase = 0.5f;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
	TMap<FName,FTransform> LastSavePoint;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		double CharacterXPBase = 100.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		int32 ProjectileAmount;

	//Excalibur Equipped Parts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Excalibur")
	UCPP_DA_Item_ExcaliburPommel* ExcaliburPommel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Excalibur")
	UCPP_DA_Item_ExcaliburGrip* ExcaliburGrip;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Inventory|Excalibur")
	UCPP_DA_Item_ExcaliburCrossguard* ExcaliburCrossguard;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Excalibur")
	UCPP_DA_Item_ExcaliburBlade* ExcaliburBlade;

   //Equipped Shield
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UCPP_DA_Item_Shield* Shield;

   //Inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TMap<UCPP_DA_Item*, int> Inventory;

protected:
	//Level Locations
UPROPERTY(EditAnywhere,Category = "Level")
	TMap<FName, FTransform> LevelLastSavePointLocation;
    
public:
	UFUNCTION(BlueprintCallable)
		void ParametersCalculation();
	UFUNCTION(BlueprintCallable)
	FTransform GetLevelLastSavePoint(FName CurrentLevel, bool &LevelVisited);
	UFUNCTION(BlueprintCallable)
	void SetNewSavedLevelTransform(FName LevelName, FTransform NewTransform);
	UFUNCTION(BlueprintCallable)
	FTransform GetSavedLevelTransform(FName LevelName, bool &ValidLevel);


private:
	UFUNCTION()
		void CalculateHealth();
	UFUNCTION()
		void CalculateStamina();
	UFUNCTION()
		void CalculateDefense();
	UFUNCTION()
		void CalculateBeast();
	UFUNCTION()
		void CalculateFracture();
	UFUNCTION()
		void CalculateRecharge();
	
};
