// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_CharaterAttachViewTarget.h"


void UCPP_CharaterAttachViewTarget::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Anim, float TotalDuration)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(MeshComp->GetWorld(), 0);
	if (PlayerController)
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative,false);
		USpringArmComponent* SpringArm = nullptr;
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
			/*Camera = Cast<UCameraComponent>(CharacterRef->FindComponentByClass(UCameraComponent::StaticClass()));
			Camera->bUsePawnControlRotation = false;
			Camera->AttachToComponent(MeshComp, AttachmentRules, BoneName);
			Camera->SetRelativeRotation(FRotator(100.0f, -55.0f, -100.0f));
			Camera->SetRelativeLocation(FVector(47.0f, -114.0f,180.0f));*/
			SpringArm = Cast<USpringArmComponent>(CharacterRef->FindComponentByClass(USpringArmComponent::StaticClass()));
			SpringArm->bDoCollisionTest = false;
			SpringArm->AttachToComponent(MeshComp, AttachmentRules, BoneName);
			

		}
	}
}

void UCPP_CharaterAttachViewTarget::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Anim)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(MeshComp->GetWorld(), 0);
	if (PlayerController)
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
		
		if (bAttachToSelfBone) {
			ACPP_DarkLifeCharacter* CharacterRef = Cast<ACPP_DarkLifeCharacter>(MeshComp->GetOwner());
			UCameraComponent* Camera = Cast<UCameraComponent>(CharacterRef->FindComponentByClass(UCameraComponent::StaticClass()));
			USpringArmComponent* SpringArm = Cast<USpringArmComponent>(CharacterRef->FindComponentByClass(USpringArmComponent::StaticClass()));
			SpringArm->AttachToComponent(MeshComp,AttachmentRules,"None");
			SpringArm->SetRelativeLocation(FVector(-12.0f, 0.0f, 188.0f));
			SpringArm->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
			SpringArm->bDoCollisionTest = true;
			//Camera->AttachToComponent(SpringArm, AttachmentRules, "SpringEndpoint");
			//Camera->ResetRelativeTransform();
			

			if (bReturnToCameraLocation) {
				CharacterRef->SetCharacterMovement(ECharacterMovement::Jog);

			}
		}
	}
}
