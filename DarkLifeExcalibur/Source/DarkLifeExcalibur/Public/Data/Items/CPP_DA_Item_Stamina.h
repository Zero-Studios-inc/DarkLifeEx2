// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Items/CPP_DA_Item.h"
#include "CPP_DA_Item_Stamina.generated.h"

/**
 * 
 */
UCLASS()
class DARKLIFEEXCALIBUR_API UCPP_DA_Item_Stamina : public UCPP_DA_Item
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		double StaminaPercent = 3.0f;

	void UseItem(ACPP_DarkLifeCharacter* CharacterRef) override;

	
};
