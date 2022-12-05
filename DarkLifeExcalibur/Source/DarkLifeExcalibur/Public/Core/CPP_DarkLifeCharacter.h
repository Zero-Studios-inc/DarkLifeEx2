// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ChildActorComponent.h"
#include "CPP_DarkLifeCharacter.generated.h"

UCLASS()
class DARKLIFEEXCALIBUR_API ACPP_DarkLifeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACPP_DarkLifeCharacter();

	//Character Parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		double Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		double MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		double Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		double MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		double Fracture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		double Recharge;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		double Defense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		double MaxBeast;

	//Character Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UChildActorComponent* Excalibur;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UChildActorComponent* LongBow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UChildActorComponent* Torch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* ShieldMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USkeletalMeshComponent* IgnisBomb;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UPointLightComponent* CharacterLight;

	//Character Animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimMontage*> EvasionAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* DrawBowAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* ReadyBowAnimation;

	//Character Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Movements")
		bool bBeastPowerMovement;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Movements")
		bool bTorchActive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Movements")
		bool bTorchUp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Movements")
		bool bDrawSword;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Movements")
		bool bDrawingSword;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Movements")
		bool bDrawShield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Movements")
		bool bDrawingShield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		double EvasionSpeedValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool bSprintKeyPress;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool bWalk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool bSprint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool bJump;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		double WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		double CrouchSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		double RunSlowSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool bSlowRun;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		double RunSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		double BeastPowerSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters|Modifiers")
		double StaminaDividerMinLimit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters|Modifiers")
		double StaminaSubstraction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters|Modifiers")
		double StaminaIncreaseDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters|Modifiers")
		double StaminaIncreaseTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters|Modifiers")
		bool bStaminaBoost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bSaveAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bIsAttacking;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bBlocking;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bDrawFinish;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bLockedEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bCanThrowProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bDrawProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bArchery;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bInvulnerability;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bCrouched;
	UPROPERTY(BlueprintReadWrite, Category = "Camera Transitions")
		AActor* CurrentViewActor;

	
	
		
protected:

	//Timers Handlers
	UPROPERTY()
		FTimerHandle StaminaIncreaseHandle;
	UPROPERTY()
		FTimerHandle StaminaDecreaseHandle;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable, Category = "Animations")
		void EvasionStepAnimations();
	UFUNCTION(BlueprintCallable, Category = "Parameters|Modifiers")
		void StaminaIncrease();
	UFUNCTION(BlueprintCallable, Category = "Parameters|Modifiers")
		void StaminaDecrease();
	UFUNCTION(BlueprintCallable, Category = "Parameters|Modifiers")
		void PoisonIncrease();
	UFUNCTION(BlueprintCallable, Category = "Parameters|Modifiers")
		void HealthIncrease(double value);
	UFUNCTION(BlueprintCallable, Category = "Attack")
		void ResetCombo();
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void StopSprint();
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void StartSprint();
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void StartSlowRun(double SurfaceDistance, double MinimumDistance ,double SpeedDecreaseFactor);
	UFUNCTION(BlueprintCallable, Category = "Attack")
		void ShootArrow();
	UFUNCTION(BlueprintCallable, Category = "Attack")
		void SetTorchActive(bool bActivate);
	UFUNCTION(BlueprintCallable, Category = "Attack")
		void ThrowDeactivate();
	UFUNCTION(BlueprintCallable, Category = "Attack")
		void ArcheryDeactivate();
	UFUNCTION(BlueprintCallable, Category = "Attack")
		bool DetectHitFromTheBack(FHitResult ReceivedHit);
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void JumpActivation(bool ActivationValue);
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void PerformJump();
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void HandWeaponsVisibility(bool hide);


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
