// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CPP_Enemy.h"

// Sets default values
ACPP_Enemy::ACPP_Enemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIState = AIDefaultState;

}

// Called when the game starts or when spawned
void ACPP_Enemy::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void ACPP_Enemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACPP_Enemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

