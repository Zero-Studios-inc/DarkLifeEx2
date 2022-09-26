// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "../Core/CPP_DarkLifeCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "CPP_CharacterChangeViewTarget.generated.h"

/**
 * 
 */
UCLASS()
class DARKLIFEEXCALIBUR_API UCPP_CharacterChangeViewTarget : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BlendTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BlendExp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EViewTargetBlendFunction> BlendFunc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bLockOutgoing = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCustomViewTarget = true;

	
};
