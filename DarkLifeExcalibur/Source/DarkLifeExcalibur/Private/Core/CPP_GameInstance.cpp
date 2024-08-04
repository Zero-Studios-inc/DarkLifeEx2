// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CPP_GameInstance.h"
#include "Core/CPP_DarkLifeCharacter.h"
#include "Core/CPP_DarkLifeSaveGame.h"

void UCPP_GameInstance::SaveGame()
{
	if (SaveGameObject) UGameplayStatics::SaveGameToSlot(SaveGameObject, SlotName, 0);
}
