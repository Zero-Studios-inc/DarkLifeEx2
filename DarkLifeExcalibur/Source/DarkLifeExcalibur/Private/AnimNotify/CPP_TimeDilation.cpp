// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_TimeDilation.h"

#include "Kismet/GameplayStatics.h"

void UCPP_TimeDilation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration)
{
	if (!bGlobalTimeDilation)
	{
		if (IsValid(MeshComp->GetOwner())) {
			MeshComp->GetOwner()->CustomTimeDilation = TimeDilationValue;
		}
	} else
	{
		UGameplayStatics::SetGlobalTimeDilation(MeshComp->GetOwner(), TimeDilationValue);
	}
}

void UCPP_TimeDilation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!bGlobalTimeDilation)
	{
		if (IsValid(MeshComp->GetOwner())) {
			MeshComp->GetOwner()->CustomTimeDilation = 1.0f;
		}
	}
		else
		{
			UGameplayStatics::SetGlobalTimeDilation(MeshComp->GetOwner(), 1.0f);
		}
	
}
