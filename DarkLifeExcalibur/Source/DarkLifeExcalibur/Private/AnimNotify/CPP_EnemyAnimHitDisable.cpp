// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_EnemyAnimHitDisable.h"

void UCPP_EnemyAnimHitDisable::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration)
{
	ACPP_Enemy* EnemyRef = Cast<ACPP_Enemy>(MeshComp->GetOwner());
	if (EnemyRef) { 
		EnemyRef->bPlayHitAnimation = false; 
		EnemyRef->SetHeavyAttackOverlayMaterial(true, HeavyAttackOverlayMaterial);
	}
}

void UCPP_EnemyAnimHitDisable::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Anim)
{
	ACPP_Enemy* EnemyRef = Cast<ACPP_Enemy>(MeshComp->GetOwner());
	if (EnemyRef) { 
		EnemyRef->bPlayHitAnimation = true;
		EnemyRef->SetHeavyAttackOverlayMaterial(false, HeavyAttackOverlayMaterial);
	}
}
