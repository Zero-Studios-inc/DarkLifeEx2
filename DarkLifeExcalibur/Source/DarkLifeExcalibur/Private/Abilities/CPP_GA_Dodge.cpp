// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/CPP_GA_Dodge.h"
#include "GameFramework/Character.h"
#include "Core/CPP_DarkLifeCharacter.h"

void UCPP_GA_Dodge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (ACPP_DarkLifeCharacter* Character = Cast<ACPP_DarkLifeCharacter>(ActorInfo->AvatarActor.Get()))
	{

		ECharacterInputDirection DodgeDirection = ECharacterInputDirection::None;
		if ((Character->GetInputAxisValue("MoveForward") > 0) && (Character->GetInputAxisValue("MoveRight") == 0))
		{
			DodgeDirection = ECharacterInputDirection::Forward;
		}
		else if ((Character->GetInputAxisValue("MoveForward") > 0) && (Character->GetInputAxisValue("MoveRight") > 0))
		{
			DodgeDirection = ECharacterInputDirection::ForwardRight;
		}
		else if ((Character->GetInputAxisValue("MoveForward") == 0) && (Character->GetInputAxisValue("MoveRight") > 0))
		{
			DodgeDirection = ECharacterInputDirection::Right;
		}
		else if ((Character->GetInputAxisValue("MoveForward") < 0) && (Character->GetInputAxisValue("MoveRight") > 0))
		{
			DodgeDirection = ECharacterInputDirection::BackwardRight;
		}
		else if ((Character->GetInputAxisValue("MoveForward") < 0) && (Character->GetInputAxisValue("MoveRight") == 0))
		{
			DodgeDirection = ECharacterInputDirection::Backward;
		}
		else if ((Character->GetInputAxisValue("MoveForward") < 0) && (Character->GetInputAxisValue("MoveRight") < 0))
		{
			DodgeDirection = ECharacterInputDirection::BackwardLeft;
		}
		else if ((Character->GetInputAxisValue("MoveForward") == 0) && (Character->GetInputAxisValue("MoveRight") < 0))
		{
			DodgeDirection = ECharacterInputDirection::Left;
		}
		else if ((Character->GetInputAxisValue("MoveForward") > 0) && (Character->GetInputAxisValue("MoveRight") < 0))
		{
			DodgeDirection = ECharacterInputDirection::ForwardLeft;
		}
		else if ((Character->GetInputAxisValue("MoveForward") == 0) && (Character->GetInputAxisValue("MoveRight") == 0))
		{
			DodgeDirection = ECharacterInputDirection::None;
		}

		Character->PlayAnimMontage(Character->EvasionAnimations[(int8)DodgeDirection], Character->EvasionSpeedValue);

		
		Character->ResetCombo();
		Character->SetCharacterMovement(ECharacterMovement::Jog);
		EndAbility(Handle,ActorInfo,ActivationInfo,true, false);
	}
}

