// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_ParryExecutionActivation.h"

void UCPP_ParryExecutionActivation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration)
{
	EnemyRef = Cast<ACPP_Enemy>(MeshComp->GetOwner());
	if (IsValid(EnemyRef)) {
		EnemyRef->bParryExecution = bActive;

		if (bActive == false) {
			
			EnemyRef->ChangeAIState(EnemyRef->GetAIPreviousState());
			EnemyRef->bForceState = false;
		}
	}
}


