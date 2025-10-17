// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Components/CPP_ItemContainer.h"
#include "Core/CPP_DarkLifeCharacter.h"
#include "Core/CPP_DarkLifeSaveGame.h"
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
	RunesSlots.Add(0, RunesSlot_00);
	RunesSlots.Add(1, RunesSlot_01);
	RunesSlots.Add(2, RunesSlot_02);
	RunesSlots.Add(3, RunesSlot_03);
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
		if (RunesSlots.Contains(index)) {
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
		if (RunesSlots.Contains(index)) {
			RunesSlots[index] = Rune;
			OnRuneAdded.Broadcast(Rune, index);
		}


	}
}

void UCPP_ItemContainer::RemoveRuneFromSlot(int index)
{
	if ((RunesSlots.Contains(index)) && IsValid(RunesSlots[index]))
	{
		RunesSlots.Add(index, nullptr);
		OnRuneRemoved.Broadcast(index);
	}
}

void UCPP_ItemContainer::SetInventoryItem(UCPP_DA_Item* Item, int ItemAmount = 1)
{
	
	if (MainInventory.Contains(Item))
	{
		
		MainInventory[Item] += ItemAmount ;
		
	}
	else
	{
		
		MainInventory.Add(Item, ItemAmount);
		
	}
	OnItemAdded.Broadcast(Item);
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
		if (MainInventory[Item] == 0) {
			MainInventory.Remove(Item);
		}
	}
	else {
		Success = false;
	}
}

bool UCPP_ItemContainer::CheckItemsExistenceInInventory(TArray<UCPP_DA_Item*> ItemsList)
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

bool UCPP_ItemContainer::IsItemEquipped(UCPP_DA_Item* Item)
{
	EItemCategory ItemCategory = Item->ItemType;
	TObjectPtr<UCPP_DA_Item_ExcaliburModule> ExcaliburModuleType = Cast<UCPP_DA_Item_ExcaliburModule>(Item);

	//This is needed in case the Item is a Rune
	TArray<TObjectPtr<UCPP_DA_Item_Rune>> AllRunes;
	RunesSlots.GenerateValueArray(AllRunes);
	TObjectPtr<UCPP_DA_Item_Rune> RuneItem = Cast<UCPP_DA_Item_Rune>(Item);

	switch (ItemCategory)
	{
	case EItemCategory::ExcaliburModule:
		
		if (IsValid(ExcaliburModuleType)) {
			EExcaliburPart ExcaliburPart = ExcaliburModuleType->ExcaliburPart;
			switch (ExcaliburPart)
			{
			case EExcaliburPart::Pommel:
				if (ExcaliburPommel == Cast<UCPP_DA_Item_ExcaliburPommel>(Item))
				{
					return true;
				}
				break;
			case EExcaliburPart::Grip:
				if (ExcaliburGrip == Cast<UCPP_DA_Item_ExcaliburGrip>(Item)) {
					return true;
				}
				break;
			case EExcaliburPart::Crossguard:
				if (ExcaliburCrossguard == Cast<UCPP_DA_Item_ExcaliburCrossguard>(Item)) {
					return true;
				}
				break;
			case EExcaliburPart::Blade:
				if (ExcaliburBlade == Cast<UCPP_DA_Item_ExcaliburBlade>(Item)) {
				return true;
			}
				break;
			default:
				break;
			}
		}
		break;
	case EItemCategory::Shield:
		if (Shield == Item)
			return true;
		break;
	case EItemCategory::Consumable:
		break;
	case EItemCategory::Rune:

		if (IsValid(RuneItem) && (AllRunes.Find(RuneItem) > -1))
		{
			return true;
		}
		break;
	case EItemCategory::Ability:
		break;
	case EItemCategory::Bow:
		if (Bow == Item)
			return true;
	default:
		break;
	}
	return false;
}

void UCPP_ItemContainer::GetItemInfo(UCPP_DA_Item* Item, UCPP_DA_Item*& ItemInfo)
{
	ItemInfo = Item;
}


void UCPP_ItemContainer::LoadSavedInfo(UCPP_DarkLifeSaveGame* SaveGame)
{
	
	if (IsValid(SaveGame)) {
		SavedGame = SaveGame;
		ExcaliburPommel = SavedGame->ExcaliburPommel;
		ExcaliburGrip = SavedGame->ExcaliburGrip;
		ExcaliburCrossguard = SavedGame->ExcaliburCrossguard;
		ExcaliburBlade = SavedGame->ExcaliburBlade;
		Shield = SavedGame->Shield;
		Bow = SavedGame->Bow;
		MainInventory = SavedGame->Inventory;
	}
		
}

