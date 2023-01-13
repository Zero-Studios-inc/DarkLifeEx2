// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CPP_Enemy.h"


// Sets default values
ACPP_Enemy::ACPP_Enemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIState = AIDefaultState;
	bCollisionActivate = true;
	MovementSpeed = 500.0;
	

}

// Called when the game starts or when spawned
void ACPP_Enemy::BeginPlay()
{
	Super::BeginPlay();
	InitHealth = Health;
	Blackboard = UAIBlueprintHelperLibrary::GetBlackboard(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	SetParameters();
	if (IsValid(UAIBlueprintHelperLibrary::GetAIController(this)))
	{
		EnemyAIController = UAIBlueprintHelperLibrary::GetAIController(this);
	}
	ChangeAIState(AIDefaultState);
	PlayerCharacterRef = Cast<ACPP_DarkLifeCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	
}


void ACPP_Enemy::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = GetMesh()->GetSocketLocation("headSocket");
	OutRotation = GetMesh()->GetSocketRotation("headSocket");
	
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

void ACPP_Enemy::SetParameters()
{

	if (IsValid(Blackboard)) {
		Blackboard->SetValueAsFloat(HealthKey, Health);
		Blackboard->SetValueAsFloat(StaminaKey, Stamina);
		Blackboard->SetValueAsFloat(MagicKey, Magic);
	}

}

void ACPP_Enemy::ChangeStateBySight()
{
	if (AIState != EAIGeneralState::Attack) {
		ChangeAIState(EAIGeneralState::Attack);
	}
}

void ACPP_Enemy::ChangeStateBySightLost()
{
	
	if (IsValid(Blackboard)) {
		Blackboard->SetValueAsObject(TargetActor, NULL);
		switch (AIState)
		{
		case EAIGeneralState::Idle:
			break;
		case EAIGeneralState::Patrol:
			break;
		case EAIGeneralState::Attack:
			Blackboard->SetValueAsVector(TargetLocation, UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation());
			break;
		case EAIGeneralState::Rest:
			break;
		case EAIGeneralState::Move:
			break;
		case EAIGeneralState::Wait:
			break;
		case EAIGeneralState::Searching:
			break;
		case EAIGeneralState::Stunt:
			break;
		case EAIGeneralState::ReceivingExecution:
			break;
		case EAIGeneralState::QuestStart:
			break;
		case EAIGeneralState::QuestEnd:
			break;
		case EAIGeneralState::Teleport:
			break;
		case EAIGeneralState::Defeated:
			break;
		default:
			break;
		}
	}
}

bool ACPP_Enemy::HitAngleInRange(FVector ImpactNormal, FVector Vector, double minAngle, double maxAngle, bool inclusiveMin, bool inclusiveMax)
{
	double dotProduct = UKismetMathLibrary::Dot_VectorVector(ImpactNormal, Vector);
	return UKismetMathLibrary::InRange_FloatFloat(UKismetMathLibrary::DegAcos(dotProduct), minAngle, maxAngle,inclusiveMin,inclusiveMax);
	
}

void ACPP_Enemy::DeathFunction()
{
	bForceState = true;
	ChangeAIState(EAIGeneralState::Wait);
	StopAnimMontage(GetCurrentMontage());
	if (IsValid(PlayerCharacterRef)) {
		if (PlayerCharacterRef->bLockedEnemy) {
			PlayerCharacterRef->UnlockTarget();
				bLockable = false;
		}
	}

	GetCharacterMovement()->StopMovementImmediately();
	SetActorEnableCollision(false);
	PlayAnimMontage(DeathAnim);
	
	
}

void ACPP_Enemy::PlaySwordHitAnimation(FVector ImpactNormal)
{
	
	if (HitAnim.Num() > 1) {
		if (HitAnim.IsValidIndex(0) && (HitAngleInRange(ImpactNormal, GetActorForwardVector(), 100.0f, 180.0f, true, true))) {
			//Back Hit Animation
			PlayAnimMontage(HitAnim[3]);
		}
		else if (HitAnim.IsValidIndex(1) && (HitAngleInRange(ImpactNormal, GetActorForwardVector(), 54.0f, 90.0f, true, true))) {

			if (HitAngleInRange(ImpactNormal, GetActorRightVector(), 0.0f, 90.0f, true, false)) {
				//Right Hit Animation
				PlayAnimMontage(HitAnim[2]);
			}
			else {
				//Left Hit Animation
				PlayAnimMontage(HitAnim[1]);
			}
		}
		else if (HitAnim.IsValidIndex(2) && (HitAngleInRange(ImpactNormal, GetActorForwardVector(), 0.0f, 44.0f, true, true)))
		{
			//Front Hit Animation
			PlayAnimMontage(HitAnim[0]);
		}
	}
	else {

		//If only have one hit animation or Front Hit Animation
		if (HitAnim.IsValidIndex(0)) {
			PlayAnimMontage(HitAnim[0]);
		}
	}
}

void ACPP_Enemy::PlayPunchHitAnimation(int32 ComboCounter)
{
	if ((!PunchDamageAnim.IsEmpty()) && (PunchDamageAnim.IsValidIndex(ComboCounter)))
	{
		PlayAnimMontage(PunchDamageAnim[ComboCounter]);
	}
}

double ACPP_Enemy::HealthDecrease(double value)
{
	return Health-=value;
}

double ACPP_Enemy::StaminaDecrease(double value)
{
	return Stamina -= value;
}


void ACPP_Enemy::ChangeAIState(EAIGeneralState NewState)
{
	if ((BTStateRelation.Find(NewState))){

		AIPreviousState = AIState;
		AIState = NewState;
		
		UBehaviorTree* NewBT = BTStateRelation.FindRef(NewState);
		UAIBlueprintHelperLibrary::GetAIController(this)->RunBehaviorTree(NewBT);
		if (IsValid(Blackboard)) {
			Blackboard->SetValueAsEnum(State, (uint8)NewState);
			if (AIState == EAIGeneralState::Attack)
			{
				GetCharacterMovement()->RotationRate = FRotator(0.0f, 90.0f, 0.0f);
			}
			else GetCharacterMovement()->RotationRate = FRotator(0.0f, 180.0f, 0.0f);

		}
	}
	
}

void ACPP_Enemy::HitAnimation(FHitResult HitInfo, ECharacterDamageType DamageType, int32 ComboCounter)
{
	FVector ImpactNormal = HitInfo.ImpactNormal;
	switch (DamageType)
	{
	case ECharacterDamageType::Sword:
		PlaySwordHitAnimation(ImpactNormal);
		break;
	case ECharacterDamageType::Shield:
		PlayAnimMontage(ShieldImpactAnim);
		break;
	case ECharacterDamageType::Torch:
		break;
	case ECharacterDamageType::Punch:
		PlayPunchHitAnimation(ComboCounter);
		break;
	default:
		PlaySwordHitAnimation(ImpactNormal);
		break;
	}
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
