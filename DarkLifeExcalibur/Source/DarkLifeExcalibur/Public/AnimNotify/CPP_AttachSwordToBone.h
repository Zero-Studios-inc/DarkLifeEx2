// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "../Core/CPP_DarkLifeCharacter.h"
#include "CPP_AttachSwordToBone.generated.h"

/**
 * 
 */
UCLASS()
class DARKLIFEEXCALIBUR_API UCPP_AttachSwordToBone : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName BoneName;
};
