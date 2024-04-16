// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_CharacterDrawSword.h"

void UCPP_CharacterDrawSword::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration)
{
	ACPP_DarkLifeCharacter* PlayerCharacter = Cast<ACPP_DarkLifeCharacter>(MeshComp->GetOwner());
	
	if (IsValid(PlayerCharacter)) {

		PlayerCharacter->CharacterDrawSword(false);
	}

}
