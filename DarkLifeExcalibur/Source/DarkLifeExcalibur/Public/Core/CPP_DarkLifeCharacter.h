// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetInputLibrary.h"
#include "Engine/EngineTypes.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ChildActorComponent.h"
#include "Components/CPP_ItemContainer.h"

class ACPP_Enemy;

#include "CPP_DarkLifeCharacter.generated.h"

#pragma once


UENUM(BlueprintType)
enum class ECharacterCombatState : uint8 {
	OneHandSword = 0 UMETA(DisplayName = "OneHandSword"),
	TwoHandSword = 1 UMETA(DisplayName = "TwoHandSword"),
	OneHandShield = 2 UMETA(DisplayName = "OneHandShield"),
	OneHandTorch = 3 UMETA(DisplayName = "OneHandTorch"),
	TwoBareHand = 4 UMETA(DisplayName = "TwoBareHand")
	
};

UENUM(BlueprintType)
enum class ECharacterFinishMoveType : uint8 {
	AfterParry = 0 UMETA(DisplayName = "AfterParry"),
	FromTheBack = 1 UMETA(DisplayName = "FromTheBack")

};

UENUM(BlueprintType)
enum class ECharacterState : uint8 {
	Normal = 0 UMETA(DisplayName = "Normal"),
	Helping = 1 UMETA(DisplayName = "Helping")
};


UENUM(BlueprintType)
enum class ECharacterDamageType : uint8 {
	Sword = 0 UMETA(DisplayName = "Sword"),
	Shield = 1 UMETA(DisplayName = "Shield"),
	Torch = 2 UMETA(DisplayName = "Torch"),
	Punch = 3 UMETA(DisplayName = "Punch"),
	Arrow = 4 UMETA(DisplaName = "Arrow")
	
};

UENUM(BlueprintType)
enum class EEnemyDamageType : uint8 {
	RegularDamage = 0 UMETA(DisplayName = "RegularDamage"),
	StrongDamage = 1 UMETA(DisplayName = "StrongDamage"),
	StuntDamage = 2 UMETA(DisplayName = "StuntDamage")


};

UENUM(BlueprintType)
enum class ECharacterMovement : uint8 {
	Walk = 0 UMETA(DisplayName = "Walk"),
	Jog = 1 UMETA(DisplayName = "Jog"),
	Sprint = 2 UMETA(DisplayName = "Sprint"),
	Crouch = 3 UMETA(DisplayName = "Crouch"),
	Dodge  = 4 UMETA (DisplayName = "Dodge"),
	Ladder = 5 UMETA(DisplayName = "Ladder"),
	Helping = 6 UMETA(DisplayName = "Helping")


};

UENUM(BlueprintType)
enum class ECharacterNegativeStatus : uint8 {
	None = 0 UMETA(DisplayName = "None"),
	Stunt = 1 UMETA(DisplayName = "Stunt"),
	Poison = 2 UMETA(DisplayName = "Poison")
	
};

UENUM(BlueprintType)
enum class ECharacterInputDirection : uint8 {
	None = 0 UMETA(DisplayName = "None"),
	Forward = 1 UMETA(DisplayName = "Forward"),
	ForwardRight = 2 UMETA(DisplayName = "ForwardRight"),
	Right = 3 UMETA(DisplayName = "Right"),
	BackwardRight = 4 UMETA(DisplayName = "BackwardRight"),
	Backward = 5 UMETA(DisplayName = "Backward"),
	BackwardLeft = 6 UMETA(DisplayName = "BackwardLeft"),
	Left = 7 UMETA(DisplayName = "Left"),
	ForwardLeft = 8 UMETA(DisplayName = "ForwardLeft")
};


UCLASS()
class DARKLIFEEXCALIBUR_API ACPP_DarkLifeCharacter : public ACharacter
{
	GENERATED_BODY()

//Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttacking);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterMovementChange, ECharacterMovement, NewCharacterMovement);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterEngaged, bool, bEngagedByEnemy, AActor*, Enemy);

public:
	// Sets default values for this character's properties
	ACPP_DarkLifeCharacter();


	//Event Dispatchers
	UPROPERTY(BlueprintAssignable, Category="Camera")
		FOnAttacking Attacking;
	UPROPERTY(BlueprintAssignable, Category = "Camera")
		FOnCharacterMovementChange CharacterMovementChange;
	UPROPERTY(BlueprintAssignable, Category = "Combat")
		FOnCharacterEngaged CharacterEngaged;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UCPP_ItemContainer* InventoryManager;

	//Character Animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimMontage*> OneHandSwordAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimMontage*> TwoHandSwordAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimMontage*> ShieldAttackAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimMontage*> TorchAttackAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimMontage*> BareHandAttackAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimMontage*> RightHandAttackAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimMontage*> EvasionAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimMontage*> AttackOnSprintAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* DrawBowAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* ReadyBowAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimMontage*> CurrentStateAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimMontage*> HitAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimMontage*> StuntAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimMontage*> OneHandParryFinisher;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimMontage*> TwoHandsParryFinisher;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimMontage*> BackFinisher;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimMontage*> OneHandChargeAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimMontage*> TwoHandsChargeAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*> OneHandDeflectedAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*> TwoHandsDeflectedAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*> ShieldDeflectedAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*> PunchDeflectedAnimations;
	UPROPERTY(EditAnywhere, BlueprintreadWrite, Category = "Animations")
		TMap<ECharacterCombatState, UAnimMontage*> BlockAnimations;
	UPROPERTY(EditAnywhere, BlueprintreadWrite, Category = "Animations")
		TMap<ECharacterCombatState, UAnimMontage*> BlockHitAnimations;

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
		bool bDrawingBow;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		ECharacterMovement CurrentCharacterMovement = ECharacterMovement::Jog;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		int ComboCounter = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bDrawShieldPreviousState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bTorchPreviousState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		double ChargeAttackKeyDownTime = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bAttackKeyPressed = false;
	UPROPERTY(BlueprintReadWrite, Category = "Attack")
		FTimerHandle ChargeAttackTimer;
	UPROPERTY(BlueprintReadWrite, Category = "Attack")
		FKey LightAttackKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool bPlayerIsEngaged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		ACPP_Enemy* EnemyAttackingRef;

	UPROPERTY(BlueprintReadWrite, Category = "Camera Transitions")
		AActor* CurrentViewActor;
	UPROPERTY(BlueprintReadWrite)
		AActor* CurrentEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		ECharacterCombatState CombatState = ECharacterCombatState::OneHandShield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		ECharacterCombatState PreviousCombatState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Status")
		ECharacterNegativeStatus CurrentCharacterNegativeStatus = ECharacterNegativeStatus::None;
		UPROPERTY(BlueprintReadWrite)
		ECharacterState CurrentCharacterState = ECharacterState::Normal;


	
	
		
protected:

	//Timers Handlers
	UPROPERTY()
		FTimerHandle StaminaIncreaseHandle;
	UPROPERTY()
		FTimerHandle StaminaDecreaseHandle;
	UPROPERTY()
		FTimerHandle ResetComboCounterHandle;
	UPROPERTY()
		bool bDrawSwordPreviousState = false;
   



	UFUNCTION()
		void UpdateStaminaByCharacterCombatState();
	UFUNCTION()
		bool HitAngleInRange(FVector ImpactNormal, FVector Vector, double minAngle, double maxAngle, bool inclusiveMin, bool inclusiveMax);
	UFUNCTION()
		void SetWalkSpeed();
	UFUNCTION()
		void SetRunSpeed();
	UFUNCTION()
		void SetCrouchSpeed();
	UFUNCTION(BlueprintCallable)
		void CheckChargeAttackKey();
	UFUNCTION(BlueprintCallable)
		void AttackFunction();
	UFUNCTION()
	void ResetComboCounter();
	

	//Input

	UFUNCTION()
		void LookUp(float value);
	UFUNCTION(BlueprintCallable)
		void Turn(float value);

	UFUNCTION()
		void Sprint();

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
		void SetCharacterMovement(ECharacterMovement NewMovement);
	/*UFUNCTION(BlueprintCallable, Category = "Movement")
		void StartSlowRun(double SurfaceDistance, double MinimumDistance, double SpeedDecreaseFactor);
		*/
	UFUNCTION(BlueprintCallable, Category = "Attack")
		void ShootArrow();
	UFUNCTION(BlueprintCallable, Category = "Attack")
		void SetTorchActive(bool bActivate, bool bRestorePreviousState);
	UFUNCTION(BlueprintCallable, Category = "Attack")
		void ThrowDeactivate();
	UFUNCTION(BlueprintCallable, Category = "Attack")
		void ArcheryDeactivate();
	UFUNCTION(BlueprintCallable, Category = "Attack")
		bool DetectHitFromTheBack(FHitResult ReceivedHit);
	UFUNCTION(BlueprintCallable, Category = "Attack")
		void UnlockTarget();
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void JumpActivation(bool ActivationValue);
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void PerformJump();
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void HandWeaponsVisibility(bool hide);
	UFUNCTION(BlueprintCallable, Category = "Attack")
		void PlayAnimationByCharacterState(int32 animationIndex, bool& Success);
	UFUNCTION(BlueprintCallable, Category = "Attack")
		void SaveComboAttack();
	UFUNCTION(BlueprintCallable, Category = "Attack")
		void SetVariablesByCombatState();
	UFUNCTION(BlueprintCallable, Category = "Attack")
		void SetCombatState(ECharacterCombatState newCombatState);
	UFUNCTION(BlueprintCallable, Category = "Animations")		
		void HitAnimation(FHitResult HitInfo, EEnemyDamageType DamageType);
	UFUNCTION(BlueprintCallable, Category = "Animations")
		void PlayRegularDamageHitAnimation(FVector ImpactNormal);
	UFUNCTION(BlueprintCallable, Category = "Animations")
		void PlayStuntDamageHitAnimation();
	UFUNCTION(BlueprintCallable, Category = "Character Status")
	void SetCharacterNegativeStatus(ECharacterNegativeStatus NewNegativeStatus);
	UFUNCTION(BlueprintCallable, Category = "Animations")
		void PlayParryFinisherAnimation(int parryIndex, UAnimMontage*& ParryMontage, TArray<UAnimMontage*>& ParryAnimList);
	UFUNCTION(BlueprintCallable, Category = "Animations")
		void PlayFromTheBackFinisherAnimation(int backFinishIndex, UAnimMontage*& FromTheBackMontage);
	UFUNCTION(BlueprintCallable, Category = "Animations")
		void PlayRandomFinishAnimation(ECharacterFinishMoveType FinishMovementType, double& AnimationLenght, bool& Success);
	UFUNCTION(BlueprintCallable, Category = "Animations")
		void PlayRandomChargeAnimationByCombatState();
	UFUNCTION(BlueprintCallable, Category = "Combat")
		void EnemyAttacking(ACPP_Enemy* Enemy);
	UFUNCTION(BlueprintCallable)
		void HideWeapons(bool bHide);
	UFUNCTION(Category = "Animations")
	void PlayDeflectedAnimation(int CustomComboIndex,ECharacterDamageType DamageType);
	UFUNCTION(BlueprintCallable, Category = "Animations")
		void PlayBlockingAnimations();
	UFUNCTION(Blueprintcallable, Category = "Animations")
		void PlayBlockingHitAnimations();
	UFUNCTION(BlueprintCallable, Category = "Character Status")
		void SetCharacterState(ECharacterState NewCharacterState);
	UFUNCTION(BlueprintCallable)
	void CharacterDrawSword(bool bOnlyToBack);
	UFUNCTION(BlueprintCallable)
	void CharacterDrawShield(bool bOnlyToBack);



	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
