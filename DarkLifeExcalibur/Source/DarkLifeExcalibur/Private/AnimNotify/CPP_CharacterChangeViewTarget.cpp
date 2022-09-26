// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_CharacterChangeViewTarget.h"

void UCPP_CharacterChangeViewTarget::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration)
{
	ACPP_DarkLifeCharacter* CharacterRef = Cast<ACPP_DarkLifeCharacter>(MeshComp->GetOwner());
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(MeshComp->GetWorld(), 0);

	if (PlayerController)
	{

		if (bCustomViewTarget)
		{

			PlayerController->SetViewTargetWithBlend(CharacterRef->CurrentViewActor, BlendTime, BlendFunc, BlendExp, bLockOutgoing);


		}

		else {
			PlayerController->SetViewTargetWithBlend(UGameplayStatics::GetPlayerCharacter(MeshComp->GetWorld(), 0), BlendTime, BlendFunc, BlendExp, bLockOutgoing);

		}

	}
}

