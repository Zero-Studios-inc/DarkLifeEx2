// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_SetFlyingMovement.h"

void UCPP_SetFlyingMovement::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration)
{
	ACharacter* CharacterRef = Cast<ACharacter>(MeshComp->GetOwner());
	if (CharacterRef) {
		CharacterRef->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		
	}
	

}

void UCPP_SetFlyingMovement::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Anim)
{
	ACharacter* CharacterRef = Cast<ACharacter>(MeshComp->GetOwner());
	if (CharacterRef) {
		CharacterRef->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	}

}