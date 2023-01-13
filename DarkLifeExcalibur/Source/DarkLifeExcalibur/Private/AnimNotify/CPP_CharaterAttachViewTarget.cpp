// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_CharaterAttachViewTarget.h"


void UCPP_CharaterAttachViewTarget::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Anim, float TotalDuration)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(MeshComp->GetWorld(), 0);
	if (PlayerController)
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, true);
		UCameraComponent* Camera;

		if (!bAttachToSelfBone)
		{
			AActor* ViewTarget = PlayerController->GetViewTarget();
			Camera = Cast<UCameraComponent>(ViewTarget->FindComponentByClass(UCameraComponent::StaticClass()));
			Camera->AttachToComponent(MeshComp, AttachmentRules, BoneName);
		}
		else 
		{
			ACPP_DarkLifeCharacter* CharacterRef = Cast<ACPP_DarkLifeCharacter>(MeshComp->GetOwner());
			Camera = Cast<UCameraComponent>(CharacterRef->FindComponentByClass(UCameraComponent::StaticClass()));
			Camera->AttachToComponent(MeshComp, AttachmentRules, BoneName);
			

		}
	}
}

void UCPP_CharaterAttachViewTarget::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Anim)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(MeshComp->GetWorld(), 0);
	if (PlayerController)
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, true);
		
		if (bAttachToSelfBone) {
			ACPP_DarkLifeCharacter* CharacterRef = Cast<ACPP_DarkLifeCharacter>(MeshComp->GetOwner());
			UCameraComponent* Camera = Cast<UCameraComponent>(CharacterRef->FindComponentByClass(UCameraComponent::StaticClass()));
			USpringArmComponent* SpringArm = Cast<USpringArmComponent>(CharacterRef->FindComponentByClass(USpringArmComponent::StaticClass()));
			Camera->AttachToComponent(SpringArm,AttachmentRules,"None");
			Camera->ResetRelativeTransform();
		}
	}
}
