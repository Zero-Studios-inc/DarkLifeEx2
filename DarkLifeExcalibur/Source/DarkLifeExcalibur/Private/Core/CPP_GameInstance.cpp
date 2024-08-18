// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CPP_GameInstance.h"
#include "Core/CPP_DarkLifeCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/UnrealType.h"
#include "UObject/PropertyPortFlags.h"

#include "Core/CPP_DarkLifeSaveGame.h"

void UCPP_GameInstance::SaveGame()
{
	UCPP_GameInstance* DLGameInstance = Cast<UCPP_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FProperty* SaveGameProperty = DLGameInstance->GetClass()->FindPropertyByName(FName(TEXT("Save Game")));
	void* SaveGamePropertyValue = SaveGameProperty->ContainerPtrToValuePtr<void>(DLGameInstance);
	UCPP_DarkLifeSaveGame* SaveGame = *reinterpret_cast<UCPP_DarkLifeSaveGame**>(SaveGamePropertyValue);

	if (SaveGame) {
		UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, 0);
	}
}

void UCPP_GameInstance::LoadGame()
{

}
