// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CPP_DarkLifeCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
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



UCLASS()
class DARKLIFEEXCALIBUR_API ACPP_Enemy : public ACharacter
{
	GENERATED_BODY()

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
	double PatrolSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double RunSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double SearchingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double MovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double MinDistanceToThrow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	double MaxDistanceToThrow;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Parameters")
	bool bCanBeExecuted;
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
		TArray <UAnimMontage*> ParryFinisher;

	
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
	
			
	
	UFUNCTION()
		void RestoringCustomTimeDilation();

	UFUNCTION()
		void SetParameters();
	UFUNCTION(BlueprintCallable)
		void ChangeStateBySight();
	UFUNCTION(BlueprintCallable)
		void ChangeStateBySightLost();
	UFUNCTION()
		bool HitAngleInRange(FVector ImpactNormal, FVector Vector,double minAngle, double maxAngle, bool inclusiveMin, bool inclusiveMax);
	UFUNCTION(BlueprintCallable)
		void DeathFunction();
	UFUNCTION()
		void PlaySwordHitAnimation(FVector ImpactNormal);
	UFUNCTION()
		void PlayPunchHitAnimation(int32 ComboCounter);
	
	
		
public:	

    UFUNCTION(BlueprintCallable, Category = "Combat|Effects")
	void HitStopEffect(double TimeDilationValue); 
	UFUNCTION(BlueprintCallable, Category = "Parameters|Modifiers")
	double HealthDecrease(double value);
	UFUNCTION(BlueprintCallable, Category = "Parameters|Modifiers")
	double StaminaDecrease(double value);
	UFUNCTION(BlueprintCallable, Category = "Parameters")
	void ChangeAIState(EAIGeneralState NewState);
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void HitAnimation(FHitResult HitInfo, ECharacterDamageType DamageType, int32 ComboCounter);

	UFUNCTION(BlueprintCallable, Category = "Animation")
		void PlayParryFinisherAnimation();
	//UFUNCTION(BlueprintCallable, Category = "Parameters")
   //void SetParameters(double InHealth, double InDamage, double InStamina, double InStaminaDamage, double InMagic, double InGiveXP, double InGiveBeastPower);

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
