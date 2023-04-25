// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Items/CPP_DA_Item_ExcaliburModule.h"
#include "CPP_DA_Item_ExcaliburBlade.generated.h"

/**
 * 
 */
UCLASS()
class DARKLIFEEXCALIBUR_API UCPP_DA_Item_ExcaliburBlade : public UCPP_DA_Item_ExcaliburModule
{
	GENERATED_BODY()

	
public:
	
	UCPP_DA_Item_ExcaliburBlade()
	{
		ExcaliburPart = EExcaliburPart::Blade;
	}

UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double Fracture;
	
	
};
