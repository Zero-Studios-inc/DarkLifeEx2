// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_AttachSwordToBone.h"

void UCPP_AttachSwordToBone::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration)
{
	ACPP_DarkLifeCharacter* PlayerCharacter = Cast<ACPP_DarkLifeCharacter>(MeshComp->GetOwner());
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true);
	if (IsValid(PlayerCharacter)) {
		PlayerCharacter->Excalibur->AttachToComponent(PlayerCharacter->GetMesh(), AttachmentRules, BoneName);
	}
}
