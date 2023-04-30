// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Items/CPP_DA_Item.h"
#include "CPP_DA_Item_Rune.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ERuneType : uint8 {
	Attack = 0 UMETA(DisplayName = "Attack"),
	Pasive = 1 UMETA(DisplayName = "Pasive")

};

UENUM(BlueprintType)
enum class ERuneMagicEffect :uint8 {
	None = 0 UMETA(DisplayName = "None"),
	Fire = 1 UMETA(DisplayName = "Fire"),
	Ice = 2 UMETA(DisplayName = "Ice")

};

UCLASS()
class DARKLIFEEXCALIBUR_API UCPP_DA_Item_Rune : public UCPP_DA_Item
{
	GENERATED_BODY()

public:

	UCPP_DA_Item_Rune() {
		ItemType = EItemCategory::Rune;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		ERuneMagicEffect MagicEffect;
	UPROPERTY(BlueprintReadOnly)
		ERuneType RuneType;

	

};

