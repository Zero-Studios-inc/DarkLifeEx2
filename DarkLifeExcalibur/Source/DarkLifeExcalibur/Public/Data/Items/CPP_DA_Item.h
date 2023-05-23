// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../../Core/CPP_DarkLifeCharacter.h"
#include "Kismet/KismetMathLibrary.h"
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
class DARKLIFEEXCALIBUR_API UCPP_DA_Item : public UPrimaryDataAsset
{
	GENERATED_BODY()




public:
UPROPERTY(BlueprintReadOnly)
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
		TSoftObjectPtr<UParticleSystem> ItemParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* ItemPicture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UTexture2D> ItemIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UStaticMesh> DropMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UStaticMesh> EquipMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundBase* Sound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<USkeletalMesh> SK_Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* UseItemActionAnimation;

	UFUNCTION(BlueprintCallable)
		virtual void UseItem(ACPP_DarkLifeCharacter* CharacterRef);
};
