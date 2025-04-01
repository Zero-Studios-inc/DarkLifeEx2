// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CPP_DA_CharacterParams.generated.h"

/**
 * 
 */
UCLASS()
class DARKLIFEEXCALIBUR_API UCPP_DA_CharacterParams : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	//Stamina
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="StaminaDecrease")
	double OneHandAttackStaminaDecrease = 0.5;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="StaminaDecrease")
	double TwoHandsAttackStaminaDecrease = 0.5;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="StaminaDecrease")
	double SprintStaminaDecrease = 0.5;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="StaminaDecrease")
	double DodgeStaminaDecrease = 0.5;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="StaminaDecrease")
	double BareHandsStaminaDecrease = 0.5;

	//Abilities
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Abilities|Lock")
	double LockRadius = 1200.0;


};
