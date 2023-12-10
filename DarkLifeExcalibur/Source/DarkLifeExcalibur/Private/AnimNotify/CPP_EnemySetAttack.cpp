// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_EnemySetAttack.h"

void UCPP_EnemySetAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration)
{
	ACPP_Enemy* EnemyRef =Cast<ACPP_Enemy> (MeshComp->GetOwner());
	if (IsValid(EnemyRef)){
		EnemyRef->SetIsInAttackAnimation(true);
	}

}

void UCPP_EnemySetAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Anim)
{
	ACPP_Enemy* EnemyRef = Cast<ACPP_Enemy>(MeshComp->GetOwner());
	if (IsValid(EnemyRef)) {
		EnemyRef->SetIsInAttackAnimation(false);
	}
}
