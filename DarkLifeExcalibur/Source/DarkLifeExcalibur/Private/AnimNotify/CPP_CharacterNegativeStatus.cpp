// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_CharacterNegativeStatus.h"

void UCPP_CharacterNegativeStatus::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration)
{
	ACPP_DarkLifeCharacter* CharacterRef = Cast<ACPP_DarkLifeCharacter>(MeshComp->GetOwner());
	if (CharacterRef) {
		CharacterRef->SetCharacterNegativeStatus(BeginStatus);
	}
}

void UCPP_CharacterNegativeStatus::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Anim)
{
	ACPP_DarkLifeCharacter* CharacterRef = Cast<ACPP_DarkLifeCharacter>(MeshComp->GetOwner());
	if (CharacterRef) {
		CharacterRef->SetCharacterNegativeStatus(EndStatus);
	}
}
