// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_TimeDilation.h"

void UCPP_TimeDilation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration)
{
	MeshComp->GetOwner()->CustomTimeDilation = TimeDilationValue;
}

void UCPP_TimeDilation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	MeshComp->GetOwner()->CustomTimeDilation = 1.0f;
}
