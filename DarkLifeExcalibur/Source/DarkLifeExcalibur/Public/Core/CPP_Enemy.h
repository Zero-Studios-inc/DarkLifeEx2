// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CPP_DarkLifeCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(BlueprintReadWrite, Category = "Parameters")
		EAIGeneralState AIState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		EAIGeneralState AIDefaultState;
	
	
		
public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
