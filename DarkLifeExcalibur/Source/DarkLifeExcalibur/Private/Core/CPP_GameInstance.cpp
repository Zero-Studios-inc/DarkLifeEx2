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

	if (SaveGame)
	{
		UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, 0);
	}
}

void UCPP_GameInstance::LoadGame()
{
}

void UCPP_GameInstance::RunesUseCoolDown(UCPP_DA_Item_Rune_Attack* AttackRune)
{
	if (!AttackRune || IsRuneOnCooldown(AttackRune))
	{
		return;
	}

	FTimerHandle CooldownTimer;
	FTimerDelegate TimerDelegate;

	TimerDelegate.BindUObject(this, &UCPP_GameInstance::OnRuneCooldownFinished, AttackRune);
	
	RunesCooldownRegistry.Add(AttackRune, CooldownTimer);
	
	GetWorld()->GetTimerManager().SetTimer(
		CooldownTimer,
		TimerDelegate,
		AttackRune->UseCooldown,
		false
	);
}

bool UCPP_GameInstance::IsRuneOnCooldown(UCPP_DA_Item_Rune_Attack* AttackRune) const
{
	return RunesCooldownRegistry.Contains(AttackRune);
}

void UCPP_GameInstance::OnRuneCooldownFinished(UCPP_DA_Item_Rune_Attack* AttackRune)
{
	RunesCooldownRegistry.Remove(AttackRune);
	OnRuneCooldownFinishedEvent.Broadcast(AttackRune);
}

float UCPP_GameInstance::GetRemainingCooldownTime(UCPP_DA_Item_Rune_Attack* AttackRune) const
{
	if (!AttackRune || !RunesCooldownRegistry.Contains(AttackRune))
	{
		return 0.0f;
	}

	const FTimerHandle& TimerHandle = RunesCooldownRegistry[AttackRune];
	return GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle);
}


