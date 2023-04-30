// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Data/Items/CPP_DA_Item.h"
#include "../../Data/Items/Runes/CPP_DA_Item_Rune_Attack.h"
#include "../../Data/Items/Runes/CPP_DA_Item_Rune_Pasive.h"
#include "Containers/Map.h"
#include "CPP_ItemContainer.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKLIFEEXCALIBUR_API UCPP_ItemContainer : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCPP_ItemContainer();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UCPP_DA_Item_Rune*> RunesSlots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<UCPP_DA_Item*, int> MainInventory;

public:
	UFUNCTION(BlueprintCallable)
	UCPP_DA_Item_Rune* GetRuneBySlotIndex(int index);
	UFUNCTION(BlueprintCallable)
		void SetRuneBySlotIndex(int index, UCPP_DA_Item_Rune* Rune);
	UFUNCTION(BlueprintCallable)
		void SetInventoryItem(UCPP_DA_Item* Item);
	UFUNCTION(BlueprintCallable)
		int GetInventoryItemAmount(UCPP_DA_Item* Item);
};
