// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "CPP_DarkLifeCharacter.h"
#include "Kismet/GameplayStatics.h"

class ACPP_DarkLifeCharacter;

#include "CPP_Enemy.generated.h"


UENUM(BlueprintType)
enum class EAIGeneralState : uint8 {
	Idle = 0 UMETA(DisplayName = "Idle"),
	Patrol = 1 UMETA(DisplayName = "Patrol"),
	Attack = 2 UMETA(DisplayName = "Attack"),
	Rest = 3 UMETA(DisplayName = "Rest"),
	Move = 4 UMETA(DisplayName = "Move"),
	Wait = 5 UMETA(DisplayName = "Wait"),
	Searching = 6 UMETA(DisplayName = "Searching"),
	Stunt = 7 UMETA(DisplayName = "Stunt"),
	ReceivingExecution = 8 UMETA(DisplayName = "ReceivingExecution"),
	QuestStart = 9 UMETA(DisplayName = "QuestStart"),
	QuestEnd = 10 UMETA(DisplayName = "QuestEnd"),
	Teleport = 11 UMETA(DisplayName = "Teleport"),
	Defeated = 12 UMETA(DisplayName = "Defeated")
	

};

UENUM(BlueprintType)
enum class EActionType : uint8 {
	None = 0 UMETA(DisplayName = "None"),
	Attacking = 1 UMETA(DisplayName = "Attacking"),
	Blocking = 2 UMETA(DisplayName = "Blocking")
};



UCLASS()
class DARKLIFEEXCALIBUR_API ACPP_Enemy : public ACharacter
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReceivingDamage);

public:
	// Sets default values for this character's properties
	ACPP_Enemy();

    //Enemy Parameters
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Parameters")
	double Health;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Parameters")
	double Stamina;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Parameters")
	double Damage;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Parameters")
	double StaminaDamage;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Parameters")
	double Magic;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Parameters")
	double GiveXP;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Parameters")
	double GiveBeastPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	TArray<ECharacterDamageType> DamageVulnerability = { ECharacterDamageType::Arrow, ECharacterDamageType::Punch, ECharacterDamageType::Shield, ECharacterDamageType::Sword, ECharacterDamageType::Torch };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	TArray<ECharacterCombatState> SwordStanceVulnerability = {ECharacterCombatState::OneHandSword, ECharacterCombatState::TwoHandSword};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double PatrolSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double RunSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double SearchingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double MovementSpeed;
	UPROPERTY(BlueprintReadWrite, Category = "Parameters")
	double TargetSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double Evasion = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double DistanceToEvade;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double DistanceToStrafe;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		double MinStrafeSideDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		double MaxStrafeSideDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double DistanceToEngage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double MinDistanceToThrow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double MaxDistanceToThrow;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Parameters")
	bool bCanBeExecuted;
	UPROPERTY(BlueprintReadWrite, Category = "Parameters")
		bool bIsInAttackAnimation;
		UPROPERTY(BlueprintReadWrite, Category = "Parameters")
		bool bIsAttacking;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		bool bCanEvade;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		bool bCanStrafe;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Parameters")
	bool bForceState;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Parameters")
	bool bBoss;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Parameters")
	bool bHiddenInGame;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Parameters")
	bool bFreeze;
	UPROPERTY(BlueprintReadWrite, Category = "Parameters")
	bool bLockable = true;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Parameters")
 	bool bCollisionActivate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		bool bCanBlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double BlockRate = 0.5f;
	UPROPERTY()
		bool bPlayHitAnimation = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		double DistanceAttackMinValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		double DistanceAttackMaxValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		bool bParry =  false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		bool bParryExecution = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		bool bExecutionActive = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		double HealthPercentExecution = 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	bool bSelfLocked = false;

	//Event Dispatchers
	UPROPERTY(BlueprintAssignable)
	FOnReceivingDamage ReceivingDamage;
    
	//AI Key Names
	UPROPERTY(BlueprintReadWrite, Category = "AI|AI Key Names")
	FName HealthKey = "Health";
	UPROPERTY(BlueprintReadWrite, Category = "AI|AI Key Names")
	FName StaminaKey = "Stamina";
	UPROPERTY(BlueprintReadWrite, Category = "AI|AI Key Names")
	FName MagicKey = "Key";
	UPROPERTY(BlueprintReadWrite, Category = "AI|AI Key Names")
	FName State = "State";
	UPROPERTY(BlueprintReadWrite, Category = "AI|AI Key Names")
	FName TargetActor = "TargetActor";
	UPROPERTY(BlueprintReadWrite, Category = "AI|AI Key Names")
	FName TargetLocation = "TargetLocation";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|AI Key Names")
	FName EnemyName;

	//AI
	UPROPERTY(BlueprintReadWrite, Category = "AI")
		AAIController* EnemyAIController;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "AI")
		TMap<EAIGeneralState, UBehaviorTree*> BTStateRelation;
	UPROPERTY(BlueprintReadWrite,BlueprintReadWrite ,Category = "Actions")
	EActionType AICurrentAction;

	//Animations
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Animations")
		TArray <UAnimMontage*> HitAnim;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Animations")
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
	

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UBlackboardComponent* Blackboard;

	UPROPERTY(BlueprintReadWrite, Category = "AI|States")
		EAIGeneralState AIState;
	UPROPERTY(BlueprintReadWrite, Category = "AI|States")
		EAIGeneralState AIPreviousState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|States")
		EAIGeneralState AIDefaultState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		double InitHealth;
	UPROPERTY()
	ACPP_DarkLifeCharacter* PlayerCharacterRef;
	UPROPERTY()
	FTimerHandle TargetSpeedInterpTimeHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAIPerceptionComponent* AIPerception;
	
			
	
	UFUNCTION()
		void RestoringCustomTimeDilation();

	UFUNCTION()
		void SetParameters();
	UFUNCTION(BlueprintCallable)
		void ChangeStateBySight();
	UFUNCTION(BlueprintCallable)
		void ChangeStateBySightLost();
	UFUNCTION(BlueprintCallable)
		bool HitAngleInRange(FVector ImpactNormal, FVector Vector,double minAngle, double maxAngle, bool inclusiveMin, bool inclusiveMax);
	UFUNCTION(BlueprintCallable)
		void DeathFunction();
	UFUNCTION()
		void PlaySwordHitAnimation(FVector ImpactNormal);
	UFUNCTION()
		void PlayPunchHitAnimation(int32 ComboCounter);
	UFUNCTION(BlueprintCallable)
		void CheckIfCanEvade();
	UFUNCTION()
		void TargetSpeedInterp();
	UFUNCTION(BlueprintCallable)
		void StartHitStop(double Duration, bool bStopPlayerCharacter);
	UFUNCTION()
		void StopHitStop();
	UFUNCTION()
	    void SetBlackboard();
    UFUNCTION()
	void DisableBlock();
	
	
		
public:	
	
    UFUNCTION(BlueprintCallable, Category = "Combat|Effects")
	void HitStopEffect(double TimeDilationValue); 
	UFUNCTION(BlueprintCallable, Category = "Parameters|Modifiers")
	double HealthDecrease(double value);
	UFUNCTION(BlueprintCallable, Category = "Parameters|Modifiers")
	double StaminaDecrease(double value);
	UFUNCTION(BlueprintCallable, Category = "Parameters|Modifiers")
	void SetTargetMovementSpeedByAIState(bool bForceRunSpeed);
	UFUNCTION(BlueprintCallable, Category = "Parameters")
	void ChangeAIState(EAIGeneralState NewState);
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void HitAnimation(FHitResult HitInfo, ECharacterDamageType DamageType, int32 ComboCounter);

	UFUNCTION(BlueprintCallable, Category = "Animation")
		void PlayParryFinisherAnimation(int parryFinishAnimation, ECharacterCombatState CharacterCombatState);
	UFUNCTION(BlueprintCallable, Category = "Animation")
		void PlayFromTheBackFinisherAnimation(int backFinishIndex);
	UFUNCTION(BlueprintCallable)
	void SetIsInAttackAnimation(bool IsAttacking);
	UFUNCTION(BlueprintCallable)
	void ChangeToSearchingState(ECharacterDamageType DamageType, ACPP_DarkLifeCharacter* CharacterRef, bool& Success);
	UFUNCTION(BlueprintCallable)
	void ReceiveDamage(FHitResult HitInfo, ACPP_DarkLifeCharacter* CharacterRef, ECharacterDamageType DamageType, double DamageReceived, int32 ComboCounter, bool &bIsInStunt,bool &bBlockSuccess ,double &HealthDecreased, USceneComponent* ExecutionIndicator);

	void ApplyDamage(bool& IsForwardHit, FHitResult& HitInfo, bool& bBlockSuccess, ECharacterDamageType DamageType, ACPP_DarkLifeCharacter* CharacterRef, bool& bSearchingSuccess, double& HealthDecreased, double DamageReceived, USceneComponent* ExecutionIndicator, double& StaminaDecreased, bool& retFlag);



	UFUNCTION()
		EAIGeneralState GetAIPreviousState();
	UFUNCTION()
	    void SetParryOverlayMaterial(bool bSetOverlayMaterial, UMaterialInterface* ParryOverlayMaterial);
	UFUNCTION()
	    void SetHeavyAttackOverlayMaterial(bool bSetOverlayMaterial, UMaterialInterface* HeavyAttackOverlayMaterial);

	//UFUNCTION(BlueprintCallable, Category = "Parameters")
   //void SetParameters(double InHealth, double InDamage, double InStamina, double InStaminaDamage, double InMagic, double InGiveXP, double InGiveBeastPower);

	//virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
