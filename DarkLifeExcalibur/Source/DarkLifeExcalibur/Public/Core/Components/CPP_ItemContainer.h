// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Data/Items/CPP_DA_Item.h"
#include "../../Data/Items/Runes/CPP_DA_Item_Rune_Attack.h"
#include "../../Data/Items/Runes/CPP_DA_Item_Rune_Pasive.h"
#include "../../Data/Items/CPP_DA_Item_ExcaliburPommel.h"
#include "../../Data/Items/CPP_DA_Item_ExcaliburGrip.h"
#include "../../Data/Items/CPP_DA_Item_ExcaliburCrossguard.h"
#include "../../Data/Items/CPP_DA_Item_ExcaliburBlade.h"
#include "../../Data/Items/CPP_DA_Item_Shield.h"
#include "../../Data/Items/CPP_DA_Item_Bow.h"
#include "Containers/Map.h"
#include "CPP_ItemContainer.generated.h"

class ACPP_DarkLifeCharacter;
class UCPP_DarkLifeSaveGame;



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKLIFEEXCALIBUR_API UCPP_ItemContainer : public UActorComponent
{
	GENERATED_BODY()

	//Event Dispatchers
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAdded, UCPP_DA_Item*, Item);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemDeleted, UCPP_DA_Item*, Item);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRuneAdded, UCPP_DA_Item_Rune*, Rune, int, Slot);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRuneRemoved, int, Slot);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExcaliburPartAdded, UCPP_DA_Item_ExcaliburModule*, ExcaliburModule);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemEquipped, UCPP_DA_Item*, Item);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExcaliburPartEquipped, UCPP_DA_Item_ExcaliburModule*, ExcaliburModule);

public:	
	// Sets default values for this component's properties
	UCPP_ItemContainer();

	UCPP_DarkLifeSaveGame* SavedGame;

	//Event Dispatchers
	UPROPERTY(BlueprintAssignable)
	FOnItemAdded OnItemAdded;
	UPROPERTY(BlueprintAssignable)
	FOnRuneAdded OnRuneAdded;
	UPROPERTY(BlueprintAssignable)
	FOnItemDeleted OnItemDeleted;
	UPROPERTY(BlueprintAssignable)
	FOnRuneRemoved OnRuneRemoved;
	UPROPERTY(BlueprintAssignable)
	FOnExcaliburPartAdded OnExcaliburPartAdded;
	UPROPERTY(BlueprintAssignable)
	FOnItemEquipped OnItemEquipped;
	UPROPERTY(BlueprintAssignable)
	FOnExcaliburPartEquipped OnExcaliburPartEquipped;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCPP_DA_Item_Shield* Shield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCPP_DA_Item_Bow* Bow;


public:

	

	//Runes Equipped Slots Management

	UFUNCTION(BlueprintCallable)
	UCPP_DA_Item_Rune* GetRuneBySlotIndex(int index);
	UFUNCTION(BlueprintCallable)
		void SetRuneBySlotIndex(int index, UCPP_DA_Item_Rune* Rune);
	UFUNCTION(BlueprintCallable)
	void RemoveRuneFromSlot(int index);


	//Inventory Management
	UFUNCTION(BlueprintCallable)
		void SetInventoryItem(UCPP_DA_Item* Item, int ItemAmount);
	UFUNCTION(BlueprintCallable)
		int GetInventoryItemAmount(UCPP_DA_Item* Item);

	UFUNCTION(BlueprintCallable)
		void DeleteItemFromInventory(UCPP_DA_Item* Item, bool& Success);

	UFUNCTION(BlueprintCallable)
		bool CheckItemsExistenceInInventory(TArray<UCPP_DA_Item*> ItemsList);
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
