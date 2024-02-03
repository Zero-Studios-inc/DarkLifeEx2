// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/CPP_Enemy.h"
#include "Core/CPP_DarkLifeCharacter.h"


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
	
	SetParameters();
	if (IsValid(UAIBlueprintHelperLibrary::GetAIController(this)))
	{
		EnemyAIController = UAIBlueprintHelperLibrary::GetAIController(this);
	}
	ChangeAIState(AIDefaultState);
	PlayerCharacterRef = Cast<ACPP_DarkLifeCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	TargetSpeed = WalkSpeed;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACPP_Enemy::SetBlackboard, 1.0f, false);
}
	
	



/*void ACPP_Enemy::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = GetMesh()->GetSocketLocation("headSocket");
	OutRotation = GetMesh()->GetSocketRotation("headSocket");
	
}*/

// Called every frame
void ACPP_Enemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//GetCharacterMovement()->MaxWalkSpeed = UKismetMathLibrary::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, TargetSpeed,DeltaTime,0.5f);
		

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

void ACPP_Enemy::ChangeStateBySight(FVector TargetDetectedLocation)
{
	if ((AIState != EAIGeneralState::Attack) && (AIState != EAIGeneralState::Stunt)) {
		
		if (IsValid(Blackboard)) {

			if (!IsValid(Blackboard->GetValueAsObject(TargetActor))){
				ChangeAIState(EAIGeneralState::Attack);
				Blackboard->SetValueAsObject(TargetActor, UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
				PlayerCharacterRef->EnemyAttacking(this);
		}
			else {
				OnAttacking.Broadcast();
			}

			Blackboard->SetValueAsVector(TargetLocation, TargetDetectedLocation);

		}

	}
}

void ACPP_Enemy::ChangeStateBySightLost()
{
	if (IsValid(PlayerCharacterRef)) {
		PlayerCharacterRef->bPlayerIsEngaged = false;
	}

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
			ChangeAIState(EAIGeneralState::Searching);
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
		PlayerCharacterRef->bPlayerIsEngaged = false;
		if (PlayerCharacterRef->EnemyAttackingRef == this)
		{
			PlayerCharacterRef = nullptr;
		}
	}

	GetCharacterMovement()->StopMovementImmediately();
	SetActorEnableCollision(false);
	PlayAnimMontage(DeathAnim);
	
	
}

void ACPP_Enemy::PlaySwordHitAnimation(FVector ImpactNormal)
{
	
	if (HitAnim.Num() > 1) {
		if (HitAnim.IsValidIndex(3) && (HitAngleInRange(ImpactNormal, GetActorForwardVector(), 100.0f, 180.0f, true, true))) {
			//Back Hit Animation
			PlayAnimMontage(HitAnim[3]);
			
		}
		else if (HitAnim.IsValidIndex(1) && (HitAngleInRange(ImpactNormal, GetActorForwardVector(), 54.0f, 90.0f, true, true))) {

			if (HitAngleInRange(ImpactNormal, GetActorRightVector(), 0.0f, 90.0f, true, false)) {
				//Right Hit Animation
				if (HitAnim.IsValidIndex(2))
				PlayAnimMontage(HitAnim[2]);
			}
			else {
				//Left Hit Animation
				PlayAnimMontage(HitAnim[1]);
			}
		}
		else if (HitAnim.IsValidIndex(0) && (HitAngleInRange(ImpactNormal, GetActorForwardVector(), 0.0f, 44.0f, true, true)))
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

void ACPP_Enemy::CheckIfCanEvade()
{
	ACPP_DarkLifeCharacter* PlayerReference = Cast<ACPP_DarkLifeCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	double DistanceToPlayer = UKismetMathLibrary::Vector_Distance(GetActorLocation(), PlayerReference->GetActorLocation());
	
	if (IsValid(PlayerReference)) {

		if (UKismetMathLibrary::InRange_FloatFloat(Evasion, 0.0f, 1.0f, true, true) && (DistanceToPlayer <= DistanceToEvade))
		{
			bCanEvade = UKismetMathLibrary::RandomBoolWithWeight(Evasion);
		}
		else {
			bCanEvade = false;
		}
	}
}

void ACPP_Enemy::TargetSpeedInterp()
{
	GetCharacterMovement()->MaxWalkSpeed = UKismetMathLibrary::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, TargetSpeed, GetWorld()->GetDeltaSeconds(), 0.5f);
	if (GetCharacterMovement()->MaxWalkSpeed >= TargetSpeed) {
		UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(GetWorld(), TargetSpeedInterpTimeHandle);
		GetCharacterMovement()->MaxWalkSpeed = TargetSpeed;
	}

}

void ACPP_Enemy::StartHitStop(double Duration, bool bStopPlayerCharacter)
{
	CustomTimeDilation = 0;
	if ((IsValid(PlayerCharacterRef)) && (bStopPlayerCharacter)) {
		PlayerCharacterRef->CustomTimeDilation = 0;
	}
	FTimerHandle HitStopTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(HitStopTimerHandle, this, &ACPP_Enemy::StopHitStop, Duration, false);
}

void ACPP_Enemy::StopHitStop()
{
	CustomTimeDilation = 1.0;
	if (IsValid(PlayerCharacterRef)) {
		PlayerCharacterRef->CustomTimeDilation = 1.0;
	}
}

void ACPP_Enemy::SetBlackboard()
{
	if (IsValid(EnemyAIController))
	Blackboard = EnemyAIController->GetBlackboardComponent();
}

void ACPP_Enemy::DisableBlock()
{
	AICurrentAction = EActionType::None;
}


double ACPP_Enemy::HealthDecrease(double value)
{
	return Health-=value;
}

double ACPP_Enemy::StaminaDecrease(double value)
{
	return Stamina -= value;
}

void ACPP_Enemy::SetTargetMovementSpeedByAIState(bool bForceRun)

{
	ACPP_DarkLifeCharacter* PlayerReference = Cast<ACPP_DarkLifeCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	double DistanceToPlayer = 0.0f;
	if (IsValid(PlayerReference)) {
		DistanceToPlayer = UKismetMathLibrary::Vector_Distance(GetActorLocation(), PlayerReference->GetActorLocation());
	}
	switch (AIState)
	{
	case EAIGeneralState::Idle:
		TargetSpeed = 0.0f;
		break;
	case EAIGeneralState::Patrol:
		TargetSpeed = WalkSpeed;
		break;
	case EAIGeneralState::Attack:
		if (!bForceRun) {
			if ((bCanStrafe) && (IsValid(PlayerReference)) && (DistanceToPlayer > DistanceToStrafe)) {
				TargetSpeed = WalkSpeed;
			}
			else if ((bCanStrafe) && (IsValid(PlayerReference)) && (DistanceToPlayer <= DistanceToStrafe)) {
				TargetSpeed = RunSpeed;
			}
			else {
				TargetSpeed = RunSpeed;
			}
		}
		else {
			TargetSpeed = RunSpeed;
		}
		
		break;
	case EAIGeneralState::Rest:
		TargetSpeed = 0.0f;
		break;
	case EAIGeneralState::Move:
		TargetSpeed = 0.0f;
		break;
	case EAIGeneralState::Wait:
		TargetSpeed = 0.0f;
		break;
	case EAIGeneralState::Searching:
		TargetSpeed = SearchingSpeed;
		break;
	case EAIGeneralState::Stunt:
		TargetSpeed = 0.0f;
		break;
	case EAIGeneralState::ReceivingExecution:
		TargetSpeed = 0.0f;
		break;
	case EAIGeneralState::QuestStart:
		TargetSpeed = 0.0f;
		break;
	case EAIGeneralState::QuestEnd:
		TargetSpeed = 0.0f;
		break;
	case EAIGeneralState::Teleport:
		TargetSpeed = 0.0f;
		break;
	case EAIGeneralState::Defeated:
		TargetSpeed = 0.0f;
		break;
	default:
		TargetSpeed = 0.0f;
		break;
	}

	
	GetWorld()->GetTimerManager().SetTimer(TargetSpeedInterpTimeHandle, this, &ACPP_Enemy::TargetSpeedInterp, GetWorld()->GetDeltaSeconds(), true);

}


void ACPP_Enemy::ChangeAIState(EAIGeneralState NewState)
{
	if ((BTStateRelation.Find(NewState))) {

		AIPreviousState = AIState;
		AIState = NewState;

		UBehaviorTree* NewBT = BTStateRelation.FindRef(NewState);
		if (IsValid(UAIBlueprintHelperLibrary::GetAIController(this))) {
			UAIBlueprintHelperLibrary::GetAIController(this)->RunBehaviorTree(NewBT);
				if (IsValid(Blackboard)) {
					Blackboard->SetValueAsEnum(State, (uint8)NewState);
						/*if (AIState == EAIGeneralState::Attack)
						{
							GetCharacterMovement()->RotationRate = FRotator(0.0f, 90.0f, 0.0f);
						}
						else GetCharacterMovement()->RotationRate = FRotator(0.0f, 180.0f, 0.0f);*/

						if ((AIPreviousState != EAIGeneralState::Stunt) && (AIState == EAIGeneralState::Stunt)) {
							bForceState = true;
						}
					if ((AIState == EAIGeneralState::Wait)) {
						StopAnimMontage(GetCurrentMontage());

					}

					SetTargetMovementSpeedByAIState(false);

				}

		}

	}
}

void ACPP_Enemy::HitAnimation(FHitResult HitInfo, ECharacterDamageType DamageType, int32 ComboCounter)
{
	SetIsInAttackAnimation(false);
	FVector ImpactNormal = HitInfo.ImpactNormal;
	if (bPlayHitAnimation) {
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
}

void ACPP_Enemy::PlayParryFinisherAnimation(int parryFinishAnimation, ECharacterCombatState CharacterCombatState)
{
	TArray<UAnimMontage*> LocalParryFinisherAnimations;

	if (CharacterCombatState == ECharacterCombatState::TwoHandSword) {
		LocalParryFinisherAnimations = TwoHandsParryFinisher;
	}

	else LocalParryFinisherAnimations = OneHandParryFinisher;

		if (LocalParryFinisherAnimations.IsValidIndex(parryFinishAnimation)) {
			if (IsValid(LocalParryFinisherAnimations[parryFinishAnimation])) {
				PlayAnimMontage(LocalParryFinisherAnimations[parryFinishAnimation]);
				
			}
		}
	}

void ACPP_Enemy::PlayFromTheBackFinisherAnimation(int backFinishIndex)
{
	if (BackFinisher.IsValidIndex(backFinishIndex)) {
		if (IsValid(BackFinisher[backFinishIndex])) {
			PlayAnimMontage(BackFinisher[backFinishIndex]);
		}
	}
}

void ACPP_Enemy::SetIsInAttackAnimation(bool IsAttacking)
{
	bIsInAttackAnimation = IsAttacking;
}

void ACPP_Enemy::ChangeToSearchingState(ECharacterDamageType DamageType, ACPP_DarkLifeCharacter* CharacterRef, bool &Success)
{
	Success = false;
	if ((DamageType == ECharacterDamageType::Arrow) && (IsValid(CharacterRef)) && (AIState != EAIGeneralState::Attack)) {
		Blackboard->SetValueAsVector(TargetLocation, CharacterRef->GetActorLocation());
		ChangeAIState(EAIGeneralState::Searching);
		Success = true;
	}
}

void ACPP_Enemy::ReceiveDamage(FHitResult HitInfo, ACPP_DarkLifeCharacter* CharacterRef, ECharacterDamageType DamageType, double DamageReceived, int32 ComboCounter, bool &bIsInStunt,bool &bBlockSuccess, double &HealthDecreased,USceneComponent* ExecutionIndicator)
{
	bool bSearchingSuccess = false;
	bool IsForwardHit;
	double StaminaDecreased;
	bIsInStunt = false;
	bBlockSuccess = false;

	if (IsValid(Blackboard)) {

				
			if (DamageVulnerability.Contains(DamageType)) {

				if ((DamageType == ECharacterDamageType::Shield) && (bParry) && (AIState != EAIGeneralState::Stunt))
				{
					ChangeAIState(EAIGeneralState::Stunt);
					bIsInStunt = true;
					return;
				}

				if ((DamageType == ECharacterDamageType::Sword) && (SwordStanceVulnerability.Contains(CharacterRef->CombatState))) {

					bool retFlag;
					ApplyDamage(IsForwardHit, HitInfo, bBlockSuccess, DamageType, CharacterRef, bSearchingSuccess, HealthDecreased, DamageReceived, ExecutionIndicator, StaminaDecreased, retFlag);
					if (retFlag) return;

				}
				else if ((DamageType == ECharacterDamageType::Sword) && (!SwordStanceVulnerability.Contains(CharacterRef->CombatState))) {
					if (IsValid(CharacterRef))
					{
						CharacterRef->PlayDeflectedAnimation(0, DamageType);
					}
				}
				else {
					bool retFlag;
					ApplyDamage(IsForwardHit, HitInfo, bBlockSuccess, DamageType, CharacterRef, bSearchingSuccess, HealthDecreased, DamageReceived, ExecutionIndicator, StaminaDecreased, retFlag);
					if (retFlag) return;
				}
			}

			
			else{
				if (IsValid(CharacterRef))
				{
					CharacterRef->PlayDeflectedAnimation(0, DamageType);
				}
			}
		}
	}


void ACPP_Enemy::ApplyDamage(bool& IsForwardHit, FHitResult& HitInfo, bool& bBlockSuccess, ECharacterDamageType DamageType, ACPP_DarkLifeCharacter* CharacterRef, bool& bSearchingSuccess, double& HealthDecreased, double DamageReceived, USceneComponent* ExecutionIndicator, double& StaminaDecreased, bool& retFlag)
{
	retFlag = true;
	IsForwardHit = HitAngleInRange(HitInfo.ImpactNormal, GetActorForwardVector(), -90.0f, 90.0f, true, true);

	if ((IsForwardHit) && bCanBlock && (AIState != EAIGeneralState::Stunt) && (!bIsInAttackAnimation)) {
		if (UKismetMathLibrary::RandomBoolWithWeight(BlockRate)) {
			if (IsValid(BlockAnim)) {
				AICurrentAction = EActionType::Blocking;
				PlayAnimMontage(BlockAnim);
				double BlockAnimationDuration = BlockAnim->RateScale * BlockAnim->GetPlayLength();
				FTimerHandle BlockHandle;
				GetWorld()->GetTimerManager().SetTimer(BlockHandle, this, &ACPP_Enemy::DisableBlock, BlockAnimationDuration, false);
				CharacterRef->PlayDeflectedAnimation(0, DamageType);
				bBlockSuccess = true;
				ChangeToSearchingState(DamageType, CharacterRef, bSearchingSuccess);
				if (bSearchingSuccess) {
					return;

				}
			}

		}
		else {
			IsForwardHit = HitAngleInRange(HitInfo.ImpactNormal, GetActorForwardVector(), 100.0f, 180.0f, true, true);
			if (!IsForwardHit) {
				return;
			}
		}
	}
	Blackboard->SetValueAsObject(TargetActor, PlayerCharacterRef);
	if (AIState == EAIGeneralState::Stunt) {
		HealthDecreased = HealthDecrease(DamageReceived);
		ReceivingDamage.Broadcast();
	}
	else {
		ChangeAIState(EAIGeneralState::Attack);
		HealthDecreased = HealthDecrease(DamageReceived);
		ReceivingDamage.Broadcast();
	}

	bExecutionActive = (HealthDecreased / InitHealth) <= HealthPercentExecution;

	if (bExecutionActive && bSelfLocked) {
		ExecutionIndicator->SetVisibility(true, true);
	}


	StaminaDecreased = StaminaDecrease(10.0f);
	Blackboard->SetValueAsFloat(StaminaKey, StaminaDecreased);
	Blackboard->SetValueAsFloat(HealthKey, HealthDecreased);
	retFlag = false;
}

EAIGeneralState ACPP_Enemy::GetAIPreviousState()
{
	return AIPreviousState;
}

void ACPP_Enemy::SetParryOverlayMaterial(bool bSetOverlayMaterial, UMaterialInterface* ParryOverlayMaterial)
{
	if (bSelfLocked) {
		if (bSetOverlayMaterial) {
			
			if (ParryOverlayMaterial) {
				GetMesh()->SetOverlayMaterial(ParryOverlayMaterial);
				CustomTimeDilation = 0.5;
			}
		}
		else {
			GetMesh()->SetOverlayMaterial(nullptr);
			CustomTimeDilation = 1.0;
		}
	}
}

void ACPP_Enemy::SetHeavyAttackOverlayMaterial(bool bSetOverlayMaterial, UMaterialInterface* HeavyAttackOverlayMaterial)
{
	if (bSelfLocked) {
		if (bSetOverlayMaterial) {
			
			if (HeavyAttackOverlayMaterial) {
				GetMesh()->SetOverlayMaterial(HeavyAttackOverlayMaterial);
				
			}
		}
		else {
			GetMesh()->SetOverlayMaterial(nullptr);
			
		}
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
