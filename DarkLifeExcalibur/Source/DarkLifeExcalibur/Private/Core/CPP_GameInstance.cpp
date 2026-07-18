// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CPP_GameInstance.h"
#include "Core/CPP_DarkLifeCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/UnrealType.h"
#include "UObject/PropertyPortFlags.h"

#include "Core/CPP_DarkLifeSaveGame.h"
#include "GameLoadLog.h"

void UCPP_GameInstance::SaveGame()
{
	UCPP_DarkLifeSaveGame* ActiveSaveGame = ResolveSaveGameObject();
	if (!ensureMsgf(IsValid(ActiveSaveGame), TEXT("SaveGame requested without a valid save object")))
	{
		UE_LOG(LogGameLoad, Error, TEXT("Save rejected: World=%s Slot=%s SaveGame=<invalid> Thread=%s"),
			*GetNameSafe(GetWorld()), *SlotName, IsInGameThread() ? TEXT("GameThread") : TEXT("OtherThread"));
		return;
	}

	UE_LOG(LogGameLoad, Log, TEXT("Save requested: World=%s Slot=%s SaveGame=%s Thread=%s"),
		*GetNameSafe(GetWorld()), *SlotName, *GetNameSafe(ActiveSaveGame),
		IsInGameThread() ? TEXT("GameThread") : TEXT("OtherThread"));

	const bool bSaved = UGameplayStatics::SaveGameToSlot(ActiveSaveGame, SlotName, 0);
	if (bSaved)
	{
		UE_LOG(LogGameLoad, Log, TEXT("Save completed: World=%s Slot=%s Result=Success"),
			*GetNameSafe(GetWorld()), *SlotName);
	}
	else
	{
		UE_LOG(LogGameLoad, Error, TEXT("Save completed: World=%s Slot=%s Result=Failure"),
			*GetNameSafe(GetWorld()), *SlotName);
	}
}

UCPP_DarkLifeSaveGame* UCPP_GameInstance::ResolveSaveGameObject() const
{
	if (IsValid(SaveGameObject))
	{
		return SaveGameObject;
	}

	// Compatibility with the existing DarkLifeGameInstance Blueprint variable.
	const FObjectPropertyBase* SaveGameProperty = CastField<FObjectPropertyBase>(
		GetClass()->FindPropertyByName(TEXT("Save Game")));
	if (!SaveGameProperty)
	{
		return nullptr;
	}

	return Cast<UCPP_DarkLifeSaveGame>(SaveGameProperty->GetObjectPropertyValue_InContainer(this));
}

void UCPP_GameInstance::LoadGame()
{
}

void UCPP_GameInstance::RunesUseCoolDown(UCPP_DA_Item_Rune* AttackRune, int RuneSlot)
{
	if (!AttackRune || IsRuneOnCooldown(RuneSlot))
	{
		return;
	}

	TObjectPtr<UCPP_DA_Item_Rune_Attack> Rune = Cast <UCPP_DA_Item_Rune_Attack> (AttackRune);

	if (!Rune) return;

	//if ()

	FTimerHandle CooldownTimer;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &UCPP_GameInstance::OnRuneCooldownFinished, RuneSlot);

	switch (RuneSlot)
	{
	case 0:
	{
		GetWorld()->GetTimerManager().SetTimer(
			RuneSlot00CoolDownTimer,
			TimerDelegate,
			Rune->UseCooldown,
			false
		);
		break;
	}
	case 1:
	{
		GetWorld()->GetTimerManager().SetTimer(
			RuneSlot01CoolDownTimer,
			TimerDelegate,
			Rune->UseCooldown,
			false
		);
		break;
	}
	case 2:
	{
		GetWorld()->GetTimerManager().SetTimer(
			RuneSlot02CoolDownTimer,
			TimerDelegate,
			Rune->UseCooldown,
			false
		);
		break;
	}
	case 3:
	{
		GetWorld()->GetTimerManager().SetTimer(
			RuneSlot03CoolDownTimer,
			TimerDelegate,
			Rune->UseCooldown,
			false
		);
		break;
	}
		
	default:
		break;
	}

	

	
	
	//RunesCooldownRegistry.Add(AttackRune, CooldownTimer);
	
	
}

bool UCPP_GameInstance::IsRuneOnCooldown(int RuneSlot) const
{
	switch (RuneSlot)
	{
	case 0:
	{
		return RuneSlot00CoolDownTimer.IsValid();
		
	}
	case 1:
	{
		return RuneSlot01CoolDownTimer.IsValid();
		
	}
	case 2:
	{
		return RuneSlot02CoolDownTimer.IsValid();
		
	}
	case 3:
	{
		return RuneSlot03CoolDownTimer.IsValid();
		
	}
	default:
		return RuneSlot00CoolDownTimer.IsValid();
	}
}

void UCPP_GameInstance::OnRuneCooldownFinished(int RuneSlot)
{
	//RunesCooldownRegistry.Remove(AttackRune);

	switch (RuneSlot)
	{
	case 0:
	{
		RuneSlot00CoolDownTimer.Invalidate();
		break;
	}
	case 1:
	{
		RuneSlot01CoolDownTimer.Invalidate();
		break;
	}
	case 2:
	{
		RuneSlot02CoolDownTimer.Invalidate();
		break;
	}
	case 3:
	{
		RuneSlot03CoolDownTimer.Invalidate();
		break;
	}
	default:
		RuneSlot00CoolDownTimer.Invalidate();
		break;
	}

	OnRuneCooldownFinishedEvent.Broadcast(RuneSlot);
}

float UCPP_GameInstance::GetRemainingCooldownTime(int RuneSlot) const
{
	switch (RuneSlot)
	{
	case 0:
	{
		return GetWorld()->GetTimerManager().GetTimerRemaining(RuneSlot00CoolDownTimer);
		
	}
	case 1:
	{
		return GetWorld()->GetTimerManager().GetTimerRemaining(RuneSlot01CoolDownTimer);
		
	}
	case 2:
	{
		return GetWorld()->GetTimerManager().GetTimerRemaining(RuneSlot02CoolDownTimer);
		

	}
	case 3:
	{

		return GetWorld()->GetTimerManager().GetTimerRemaining(RuneSlot03CoolDownTimer);
		
	}
	default:
		return GetWorld()->GetTimerManager().GetTimerRemaining(RuneSlot00CoolDownTimer);
	}

	
	
}

float UCPP_GameInstance::GetRuneRemainingTime(int RuneSlot)
{
	float RemainingTime = 0.0f;

	switch (RuneSlot)
	{
	case 0: 
	{
		if (RuneSlot00CoolDownTimer.IsValid()) {
			RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(RuneSlot00CoolDownTimer);
		}
		
		break;
	}
	case 1:
	{
		if (RuneSlot01CoolDownTimer.IsValid()) {
			RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(RuneSlot01CoolDownTimer);
		}
		break;
	}
	case 2:
	{
		if (RuneSlot02CoolDownTimer.IsValid()) {
			RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(RuneSlot02CoolDownTimer);
		}
		break;
	}
	case 3:
	{
		if (RuneSlot03CoolDownTimer.IsValid()) {
			RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(RuneSlot03CoolDownTimer);
		}
		break;
	}
		

	default:
		break;
	}

	return RemainingTime;
}



