// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Data/Spells/CPP_DA_ProjectileSpell.h"
#include "CPP_ProjectileSpell.generated.h"

UCLASS()
class DARKLIFEEXCALIBUR_API ACPP_ProjectileSpell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_ProjectileSpell();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpellSettings", meta = (ExposeOnSpawn = "true"))
	UCPP_DA_ProjectileSpell* ProjectileSpellSettings;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//Components
	UPROPERTY(BlueprintReadOnly,VisibleAnywhere, Category = "Components")
	TObjectPtr<UNiagaraComponent> Spell;
	UPROPERTY(BlueprintReadOnly,VisibleAnywhere, Category = "Components")
	TObjectPtr<UNiagaraComponent> SpellHit;
	UPROPERTY(BlueprintReadWrite, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	UPROPERTY(BlueprintReadWrite,VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> SphereCollision;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Components")
	TObjectPtr<UAudioComponent> SpellSoundComponent;

	//Projectile Spell Settings
	UPROPERTY(BlueprintReadOnly, Category="SpellSettings")
	FRotator SpellRotation;
	UPROPERTY(BlueprintReadOnly, Category = "SpellSettings")
	FVector SpellScale;
	UPROPERTY(BlueprintReadOnly, Category = "SpellSettings")
	FRotator SpellHitRotation;
	UPROPERTY(BlueprintReadOnly, Category = "SpellSettings")
	FVector SpellHitScale;
	UPROPERTY(BlueprintReadOnly, Category = "SpellSettings")
	double SpellDamage = 5.0f;
	UPROPERTY(BlueprintReadOnly, Category = "SpellSettings")
	double SpellImpulse = 2700.0F;
	UPROPERTY(BlueprintReadOnly, Category = "SpellSettings")
	double SpellStaminaDamage = 40.0f;
	UPROPERTY(BlueprintReadOnly, Category = "SpellSettings")
	TObjectPtr<USoundBase> SpellSound;
	UPROPERTY(BlueprintReadOnly, Category="SpellSettings")
	TObjectPtr<USoundBase> SpellHitSound;

UFUNCTION(BlueprintCallable)
	void OnProjectileSpellHit();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
