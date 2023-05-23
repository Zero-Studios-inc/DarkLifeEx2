// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Data/Items/CPP_DA_Item.h"
#include "../../Core/CPP_DarkLifeSaveGame.h"
#include "../../Data/Items/Runes/CPP_DA_Item_Rune_Attack.h"
#include "../../Data/Items/Runes/CPP_DA_Item_Rune_Pasive.h"
#include "../../Data/Items/CPP_DA_Item_ExcaliburPommel.h"
#include "../../Data/Items/CPP_DA_Item_ExcaliburGrip.h"
#include "../../Data/Items/CPP_DA_Item_ExcaliburCrossguard.h"
#include "../../Data/Items/CPP_DA_Item_ExcaliburBlade.h"
#include "Containers/Map.h"
#include "CPP_ItemContainer.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKLIFEEXCALIBUR_API UCPP_ItemContainer : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCPP_ItemContainer();

	UCPP_DarkLifeSaveGame* SavedGame;

	

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

	//Excalibur Equipped Parts
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCPP_DA_Item_ExcaliburPommel* ExcaliburPommel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCPP_DA_Item_ExcaliburGrip* ExcaliburGrip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCPP_DA_Item_ExcaliburCrossguard* ExcaliburCrossguard;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCPP_DA_Item_ExcaliburBlade* ExcaliburBlade;


public:

	

	//Runes Equipped Slots Management

	UFUNCTION(BlueprintCallable)
	UCPP_DA_Item_Rune* GetRuneBySlotIndex(int index);
	UFUNCTION(BlueprintCallable)
		void SetRuneBySlotIndex(int index, UCPP_DA_Item_Rune* Rune);


	//Inventory Management
	UFUNCTION(BlueprintCallable)
		void SetInventoryItem(UCPP_DA_Item* Item);
	UFUNCTION(BlueprintCallable)
		int GetInventoryItemAmount(UCPP_DA_Item* Item);

	UFUNCTION(BlueprintCallable)
		void DeleteItemFromInventory(UCPP_DA_Item* Item, bool& Success);

	UFUNCTION(BlueprintCallable)
		bool CheckItemsExitenceInInventory(TArray<UCPP_DA_Item*> ItemsList);
	UFUNCTION(BlueprintCallable)
		void DeleteItemsListFromInventory(TArray<UCPP_DA_Item*> ItemsList);
	UFUNCTION(BlueprintCallable)
		bool IsItemEquipped(UCPP_DA_Item* Item);
	UFUNCTION(BlueprintCallable)
		void GetItemInfo(UCPP_DA_Item* Item, UCPP_DA_Item*& ItemInfo);

	//Load Saved Info
	UFUNCTION(BlueprintCallable)
	void LoadSavedInfo(UCPP_DarkLifeSaveGame* SaveGame);
};
