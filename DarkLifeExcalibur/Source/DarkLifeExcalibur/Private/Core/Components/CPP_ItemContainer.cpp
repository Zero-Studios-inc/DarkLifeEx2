// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Components/CPP_ItemContainer.h"

// Sets default values for this component's properties
UCPP_ItemContainer::UCPP_ItemContainer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCPP_ItemContainer::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCPP_ItemContainer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UCPP_DA_Item_Rune* UCPP_ItemContainer::GetRuneBySlotIndex(int index)
{
	if ((index >= 0) && (index < 4))
	{
		if (RunesSlots.IsValidIndex(index)) {
			return RunesSlots[index];
		}
		else return nullptr;
	}
	else return nullptr;
}

void UCPP_ItemContainer::SetRuneBySlotIndex(int index, UCPP_DA_Item_Rune* Rune)
{
	if ((index >= 0) && (index < 4))
	{
		if (RunesSlots.IsValidIndex(index)) {
			RunesSlots[index] = Rune;
		}


	}
}

void UCPP_ItemContainer::SetInventoryItem(UCPP_DA_Item* Item)
{
	int Value = 0;
	if (MainInventory.Contains(Item))
	{
		
		MainInventory[Item]++;
		Value = MainInventory[Item];
	}
	else
	{
		
		MainInventory.Add(Item, 1);
		Value = 1;
	}
}


int UCPP_ItemContainer::GetInventoryItemAmount(UCPP_DA_Item* Item)
{
	if ((MainInventory.Contains(Item)))
	{
		int Amount = 0;
			Amount = MainInventory[Item];
			return Amount;
	}
	else return -1;
}

void UCPP_ItemContainer::DeleteItemFromInventory(UCPP_DA_Item* Item, bool& Success)
{
	if ((MainInventory.Contains(Item))) {
		MainInventory[Item]--;
		Success = true;
		if ((MainInventory[Item] == 0)) {
			MainInventory.Remove(Item);
		}
	}
	else {
		Success = false;
	}
}

bool UCPP_ItemContainer::CheckItemsExitenceInInventory(TArray<UCPP_DA_Item*> ItemsList)
{
	if ((!ItemsList.IsEmpty()) && (!MainInventory.IsEmpty())) {
		for (size_t i = 0; i < ItemsList.Num(); i++)
		{
			if (GetInventoryItemAmount(ItemsList[i]) < 0) {
				return false;
			}
		}
		return true;
	}
	else return false;
	
}

void UCPP_ItemContainer::DeleteItemsListFromInventory(TArray<UCPP_DA_Item*> ItemsList)
{
	bool Success;
	if ((!ItemsList.IsEmpty()) && (!MainInventory.IsEmpty())) {
		for (size_t i = 0; i < ItemsList.Num(); i++)
		{
			DeleteItemFromInventory(ItemsList[i],Success);
		}
	}
}

