// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_CharaterAttachViewTarget.h"


void UCPP_CharaterAttachViewTarget::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Anim, float TotalDuration)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(MeshComp->GetWorld(), 0);
	if (PlayerController)
	{
		AActor* ViewTarget = PlayerController->GetViewTarget();
		UCameraComponent* Camera = Cast<UCameraComponent>(ViewTarget->FindComponentByClass(UCameraComponent::StaticClass()));
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, true);
		Camera->AttachToComponent(MeshComp, AttachmentRules, BoneName);
	}
}
