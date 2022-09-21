// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CPP_Enemy.h"

// Sets default values
ACPP_Enemy::ACPP_Enemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIState = AIDefaultState;
	bCollisionActivate = true;
	

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

void ACPP_Enemy::RestoringCustomTimeDilation()
{
	CustomTimeDilation = 1.0;
	UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->CustomTimeDilation = 1.0;
}

double ACPP_Enemy::HealthDecrease(double value)
{
	return Health-=value;
}

void ACPP_Enemy::HitStopEffect(double TimeDilationValue){
	CustomTimeDilation = TimeDilationValue;
	UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->CustomTimeDilation = TimeDilationValue;
	FTimerHandle RestoringTimeDilationHandle;
	GetWorld()->GetTimerManager().SetTimer(RestoringTimeDilationHandle, this, &ACPP_Enemy::RestoringCustomTimeDilation, 0.1f, false);
}

/*void ACPP_Enemy::SetParameters(double InHealth, double InDamage, double InStamina, double InStaminaDamage, double InMagic, double InGiveXP, double InGiveBeastPower)
{
	Health = InHealth;
	Damage = InDamage;
	Stamina = InStamina;
	StaminaDamage = InStaminaDamage;
	Magic = InMagic;
	GiveXP = InGiveXP;
	GiveBeastPower = InGiveBeastPower;

	//UBlackboardComponent* EnemyBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(this);
	//EnemyBlackboard->SetValueAsFloat(HealthKey, UKismetMathLibrary::Conv_DoubleToFloat(Health));
	//EnemyBlackboard->SetValueAsFloat(StaminaKey, UKismetMathLibrary::Conv_DoubleToFloat(Stamina));
	//EnemyBlackboard->SetValueAsFloat(MagicKey, UKismetMathLibrary::Conv_DoubleToFloat(Magic));
	
	
}*/
