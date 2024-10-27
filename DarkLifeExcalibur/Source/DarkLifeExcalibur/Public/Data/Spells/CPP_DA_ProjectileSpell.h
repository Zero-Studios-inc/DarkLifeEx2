// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "NiagaraFunctionLibrary.h"
#include "CPP_DA_ProjectileSpell.generated.h"

/**
 * 
 */
UCLASS()
class DARKLIFEEXCALIBUR_API UCPP_DA_ProjectileSpell : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	//Settings
UPROPERTY(EditAnywhere, BlueprintReadWrite)
UNiagaraSystem* Spell;
UPROPERTY(EditAnywhere, BlueprintReadWrite)
UNiagaraSystem* SpellHit;
UPROPERTY(EditAnywhere, BlueprintReadWrite)
double SpellImpulse = 2700.0f;
UPROPERTY(EditAnywhere, BlueprintReadWrite)
double SpellDamage = 5.0f;
UPROPERTY(EditAnywhere, BlueprintReadWrite)
double SpellStaminaDamage = 40.0f;
UPROPERTY(EditAnywhere, BlueprintReadWrite)
double SphereCollisionRadius = 32.0f;
UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* SpellHitSound;
	
	
};
