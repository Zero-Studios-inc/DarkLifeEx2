// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Items/Runes/CPP_DA_Item_Rune.h"
#include "CPP_DA_Item_Rune_Attack.generated.h"

/**
 * 
 */
UCLASS()
class DARKLIFEEXCALIBUR_API UCPP_DA_Item_Rune_Attack : public UCPP_DA_Item_Rune
{
	GENERATED_BODY()

public:
	UCPP_DA_Item_Rune_Attack() {
		RuneType = ERuneType::Attack;
	}
};
