// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_ToggleInput.h"
#include "Core/CPP_DarkLifeCharacter.h"

void UCPP_ToggleInput::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Anim, float TotalDuration)
{
	if (MeshComp) {
		ACPP_DarkLifeCharacter* DLCharacter = Cast<ACPP_DarkLifeCharacter>(MeshComp->GetOwner());
		if (DLCharacter)
		{
			if (bEnable)
			{
				DLCharacter->EnableInput(Cast<APlayerController>(DLCharacter->GetController()));
			}
			else
			{
				DLCharacter->DisableInput(Cast<APlayerController>(DLCharacter->GetController()));
			}
		}
	}
}

