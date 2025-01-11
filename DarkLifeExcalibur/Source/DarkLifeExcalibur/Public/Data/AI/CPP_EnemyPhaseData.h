// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelSequence.h"
#include "CPP_EnemyPhaseData.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EEnemyPhase : uint8 {
	Phase_0 = 0 UMETA(DisplayName = "Phase 0"),
	Phase_1 = 1 UMETA(DisplayName = "Phase 1"),
	Phase_2 = 0 UMETA(DisplayName = "Phase 2")
};


UCLASS()
class DARKLIFEEXCALIBUR_API UCPP_EnemyPhaseData : public UDataAsset
{
	GENERATED_BODY()

public:

	//LevelSequence
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Phase Sequence")
	ULevelSequence* PhaseStartSequence;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Phase Sequence")
	bool bPlayPhaseSequence = false;
	
	//Animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray <UAnimMontage*> HitAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray <UAnimMontage*> PunchDamageAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray <UAnimMontage*> AttackAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray <UAnimMontage*> EvadeAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* BlockAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* DeathAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* ThrowAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* StuntAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* ShieldImpactAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray <UAnimMontage*> DistanceAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray <UAnimMontage*> OneHandParryFinisher;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray <UAnimMontage*> TwoHandsParryFinisher;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*> BackFinisher;


};
