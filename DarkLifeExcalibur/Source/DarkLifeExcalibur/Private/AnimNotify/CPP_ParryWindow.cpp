// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_ParryWindow.h"

void UCPP_ParryWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration)
{
	ACPP_Enemy* EnemyRef = Cast<ACPP_Enemy>(MeshComp->GetOwner());
	if (IsValid(EnemyRef)) {
		EnemyRef->bParry = true;
		EnemyRef->SetParryOverlayMaterial(true);
	}
}

void UCPP_ParryWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ACPP_Enemy* EnemyRef = Cast<ACPP_Enemy>(MeshComp->GetOwner());
	if (IsValid(EnemyRef)) {
		EnemyRef->bParry = false;
		EnemyRef->SetParryOverlayMaterial(false);
	}
}
