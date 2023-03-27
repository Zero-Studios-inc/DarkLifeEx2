// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_ParryExecutionActivation.h"

void UCPP_ParryExecutionActivation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration)
{
	EnemyRef = Cast<ACPP_Enemy>(MeshComp->GetOwner());
	if (IsValid(EnemyRef)) {
		EnemyRef->bParryExecution = true;
	}
}

void UCPP_ParryExecutionActivation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (IsValid(EnemyRef)) {
		EnemyRef->bParryExecution = false;
	}
}
