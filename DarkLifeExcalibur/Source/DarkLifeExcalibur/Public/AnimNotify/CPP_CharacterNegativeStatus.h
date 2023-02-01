// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "../Core/CPP_DarkLifeCharacter.h"
#include "CPP_CharacterNegativeStatus.generated.h"

/**
 * 
 */
UCLASS()
class DARKLIFEEXCALIBUR_API UCPP_CharacterNegativeStatus : public UAnimNotifyState
{
	GENERATED_BODY()
		virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration) override;
	    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Anim) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ECharacterNegativeStatus BeginStatus = ECharacterNegativeStatus::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ECharacterNegativeStatus EndStatus = ECharacterNegativeStatus::None;

};
