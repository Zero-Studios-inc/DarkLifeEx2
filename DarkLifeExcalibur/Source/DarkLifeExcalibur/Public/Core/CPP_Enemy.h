// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CPP_DarkLifeCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "CPP_Enemy.generated.h"

UENUM(BlueprintType)
enum class EAIGeneralState : uint8 {
	Idle = 0 UMETA(DisplayName = "Idle"),
	Patrol = 1 UMETA(DisplayName = "Patrol"),
	Attack = 2 UMETA(DisplayName = "Attack"),
	Rest = 3 UMETA(DisplayName = "Rest"),
	Move = 4 UMETA(DisplayName = "Move")

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
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Parameters")
	bool bCanBeExecuted;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Parameters")
	bool bInFloor;
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
    
	//AI Key Names
	UPROPERTY(BlueprintReadWrite, Category = "AI|AI Key Names")
	FName HealthKey;
	UPROPERTY(BlueprintReadWrite, Category = "AI|AI Key Names")
	FName StaminaKey;
	UPROPERTY(BlueprintReadWrite, Category = "AI|AI Key Names")
	FName MagicKey;
	UPROPERTY(BlueprintReadWrite, Category = "AI|AI Key Names")
	FName EnemyName;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(BlueprintReadWrite, Category = "AI|States")
		EAIGeneralState AIState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|States")
		EAIGeneralState AIDefaultState;
	//UPROPERTY(BlueprintReadWrite, Category = "AI")
		//AAIController* EnemyController;
	
	
		
public:	

    UFUNCTION(BlueprintCallable, Category = "Combat|Effects")
	void HitStopEffect(); 
	//UFUNCTION(BlueprintCallable, Category = "Parameters")
   //void SetParameters(double InHealth, double InDamage, double InStamina, double InStaminaDamage, double InMagic, double InGiveXP, double InGiveBeastPower);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
