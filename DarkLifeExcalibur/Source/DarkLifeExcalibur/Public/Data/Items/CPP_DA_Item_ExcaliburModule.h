// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Items/CPP_DA_Item.h"
#include "CPP_DA_Item_ExcaliburModule.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EExcaliburPart : uint8 {
	Pommel = 0 UMETA(DisplayName = "Pommel"),
	Grip = 1 UMETA(DisplayName = "Grip"),
	Crossguard = 2 UMETA(DisplayName = "Crossguard"),
	Blade = 3 UMETA(DisplayName = "Blade")
	



};


UCLASS()
class DARKLIFEEXCALIBUR_API UCPP_DA_Item_ExcaliburModule : public UCPP_DA_Item
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ID;

protected:
	UPROPERTY(BlueprintReadOnly)
		EExcaliburPart ExcaliburPart;
	
	

	
};
