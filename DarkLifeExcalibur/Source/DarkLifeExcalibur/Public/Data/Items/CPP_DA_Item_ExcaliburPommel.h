// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Items/CPP_DA_Item_ExcaliburModule.h"
#include "CPP_DA_Item_ExcaliburPommel.generated.h"

/**
 * 
 */
UCLASS()
class DARKLIFEEXCALIBUR_API UCPP_DA_Item_ExcaliburPommel : public UCPP_DA_Item_ExcaliburModule
{
	GENERATED_BODY()
	
public:
	UCPP_DA_Item_ExcaliburPommel()
	{
		ExcaliburPart = EExcaliburPart::Pommel;
	}
};
