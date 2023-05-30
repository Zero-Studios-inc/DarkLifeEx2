// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "../Core/CPP_DarkLifeCharacter.h"
#include "CPP_CharaterAttachViewTarget.generated.h"

/**
 * 
 */
UCLASS()
class DARKLIFEEXCALIBUR_API UCPP_CharaterAttachViewTarget : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName BoneName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bAttachToSelfBone = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bReturnToCameraLocation = true;
    	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Anim, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Anim) override;
	
};


