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
#include "../Data/Items/CPP_DA_Item_Bow.h"
#include "Core/Components/CPP_ItemContainer.h"
#include "Containers/Map.h"
#include "LevelSequence.h"
#include "Core/CPP_DarkLifeCharacter.h"
#include "CPP_DarkLifeSaveGame.generated.h"


class ACPP_DarkLifeCharacter;
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
		UPROPERTY(BlueprintReadWrite, Category = "Character")
		ECharacterState CharacterState = ECharacterState::Injuried;

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

	//Equipped Bow
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UCPP_DA_Item_Bow* Bow;

   //Inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TMap<TObjectPtr<UCPP_DA_Item>, int> Inventory;
	
	


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> ItemBoxRegister;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform CheckPointTransform;

	//Missions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missions")
	TMap<TSubclassOf<AActor>, bool> MissionActivation;

	//Missions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missions")
	int CurrentMission;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missions")
	TArray<int> MissionInfoActivation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missions")
	TArray<bool> MissionComplete;

	//Cinematics
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinematics")
	TMap<ULevelSequence*, bool> CinematicActivation;

	//Tutorials
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorials")
	TArray<int> TutorialsDiscovered;

	//Beated Enemies
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemies")
	TArray<FGameplayTag> DefeatedEnemies;
	

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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tutorials")
	void IstutorialDiscovered(int TutorialID, bool& bDiscovered);
	UFUNCTION(BlueprintCallable, Category = "Tutorials")
	void AddTutorialDiscovered(int TutorialID);


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
