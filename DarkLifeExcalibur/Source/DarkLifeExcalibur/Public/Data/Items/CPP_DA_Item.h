// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CPP_DA_Item.generated.h"

/**
 * 
 */
UENUM(BlueprintType) 
enum class EItemCategory : uint8 {
	ExcaliburModule = 0 UMETA(DisplayName = "ExcaliburModule"),
	Shield = 1 UMETA(DisplayName = "Shield"),
	Consumable = 2 UMETA(DisplayName = "Consumable"),
	Rune = 3 UMETA(DisplayName = "Rune"),
	Ability = 4 UMETA(DisplayName = "Ability")
	


};


UCLASS()
class DARKLIFEEXCALIBUR_API UCPP_DA_Item : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		EItemCategory ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ItemDescription;
	UPROPERTY(EditAnywhere, BLueprintReadWrite)
		double ItemSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FColor LightColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UParticleSystem* ItemParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* ItemPicture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* ItemIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMesh* DropMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMesh* EquipMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundBase* Sound;

	
};
