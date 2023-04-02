// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_CharacterParryDetection.h"

void UCPP_CharacterParryDetection::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration)
{
	ChracterRef = Cast<ACPP_DarkLifeCharacter>(MeshComp->GetOwner());
	if (IsValid(ChracterRef)) {
		CurrentEnemy = Cast<ACPP_Enemy>(ChracterRef->CurrentEnemy);
		if (IsValid(CurrentEnemy)) {

			//Something is missing but now I don't know
		}

	}
}

void UCPP_CharacterParryDetection::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if (IsValid(CurrentEnemy)) {
		if (CurrentEnemy->bParry == true)
		{
			//CurrentEnemy->ChangeAIState(EAIGeneralState::Stunt);

			//Print if Parry Happened
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("PARRY, PARRY, PARRY"), true, FVector2D(2.f, 2.f));

		
		}
	}
}
