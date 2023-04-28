// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/CPP_DarkLifeCharacter.h"
#include "Core/CPP_Enemy.h"


// Sets default values
ACPP_DarkLifeCharacter::ACPP_DarkLifeCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	
	//Parameters Default Values
	Health = 100;
	MaxHealth = 100;
	Stamina = 100;
	MaxStamina = 100;
	Fracture = 10;
	Recharge = 10;
	Defense = 10;

	//Components Init
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->SetRelativeLocation(FVector(-12.0f, 0.0f, 188.0f ));
	SpringArm->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->TargetArmLength = 200.0f;

	
	LongBow = CreateDefaultSubobject<UChildActorComponent>(TEXT("Bow"));
	LongBow->SetupAttachment(GetMesh(), "Bow_Back");

	IgnisBomb = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Ignis"));
	IgnisBomb->SetupAttachment(GetMesh(), "ignis");
	IgnisBomb->SetVisibility(false, false);
	
	Excalibur = CreateDefaultSubobject<UChildActorComponent>(TEXT("Excalibur"));
	Excalibur->SetupAttachment(GetMesh(), "Sword_Back");

	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shield"));
	ShieldMesh->SetupAttachment(GetMesh(), "Shield_Back");

	Torch = CreateDefaultSubobject<UChildActorComponent>(TEXT("Torch"));
	Torch->SetupAttachment(GetMesh(), "Torch");
	Torch->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	Torch->SetHiddenInGame(true, true);

	CharacterLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("CharacterLight"));
	CharacterLight->SetupAttachment(RootComponent);
	

	//Variables Default Values
	EvasionSpeedValue = 1.5;
	bBeastPowerMovement = false;
	bSaveAttack = false;
	bIsAttacking = false;
	bDrawFinish = false;
	bBlocking = false;
	StaminaDividerMinLimit = -4.0;
	StaminaSubstraction = 12.0;
	StaminaIncreaseDelay = 1.0;
	StaminaIncreaseTime = 0.01;
	bSprintKeyPress = false;
	bWalk = false;
	WalkSpeed = 120.0;
	//RunSlowSpeed = 100;
	RunSpeed = 312.0;
	BeastPowerSpeed = 5000.0;
	bSprint = false;
	bLockedEnemy = false;
	bDrawingShield = false;
	bDrawShield = false;
	bDrawSword = false;
	bDrawingSword = false;
	bDrawingBow = false;
	bTorchActive = false;
	bJump = false;
	//bSlowRun = false;
	bStaminaBoost = false;
	bTorchUp = false;
	bInvulnerability = false;
	bCrouched = false;
	CrouchSpeed = 20.0;
	
	
}

void ACPP_DarkLifeCharacter::UpdateStaminaByCharacterCombatState()
{
	switch (CombatState)
	{
	case ECharacterCombatState::OneHandSword:
		Stamina = UKismetMathLibrary::FClamp(Stamina - (MaxStamina / 6.0), 0.0, MaxStamina);
		break;
	case ECharacterCombatState::TwoHandSword:
		Stamina = UKismetMathLibrary::FClamp(Stamina - (MaxStamina / 3.0), 0.0, MaxStamina);
		break;
	case ECharacterCombatState::OneHandShield:
		break;
	case ECharacterCombatState::OneHandTorch:
		break;
	case ECharacterCombatState::TwoBareHand:
		Stamina = UKismetMathLibrary::FClamp(Stamina - (MaxStamina / 12.0), 0.0, MaxStamina);
		break;
	default:
		break;
	}
}

bool ACPP_DarkLifeCharacter::HitAngleInRange(FVector ImpactNormal, FVector Vector, double minAngle, double maxAngle, bool inclusiveMin, bool inclusiveMax)
{
	double dotProduct = UKismetMathLibrary::Dot_VectorVector(ImpactNormal, Vector);
	return UKismetMathLibrary::InRange_FloatFloat(UKismetMathLibrary::DegAcos(dotProduct), minAngle, maxAngle, inclusiveMin, inclusiveMax);
	
}

void ACPP_DarkLifeCharacter::SetWalkSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ACPP_DarkLifeCharacter::SetCrouchSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
}

void ACPP_DarkLifeCharacter::CheckChargeAttackKey()
{
	bool bKeyDownTimeCheck = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetInputKeyTimeDown(LightAttackKey) >= ChargeAttackKeyDownTime;
	if ((UKismetInputLibrary::Key_IsValid(LightAttackKey)) && bKeyDownTimeCheck) {
		if (!bIsAttacking) {
			PlayRandomChargeAnimationByCombatState();
			UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(GetWorld(), ChargeAttackTimer);
		}
		else {
			UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(GetWorld(), ChargeAttackTimer);
		}

	
	}
}

void ACPP_DarkLifeCharacter::AttackFunction()
{
	if (Stamina >= 20.0f) {

		UKismetSystemLibrary::K2_PauseTimer(this, "StaminaIncrease");

		if (bIsAttacking) {
			bSaveAttack = true;
		}
		else {
			bIsAttacking = true;
			bAttackKeyPressed = true;
			bool bAttackSuccess;
			PlayAnimationByCharacterState(ComboCounter, bAttackSuccess);
			SetCharacterMovement(ECharacterMovement::Jog);
			GetWorldTimerManager().SetTimer(ChargeAttackTimer, this, &ACPP_DarkLifeCharacter::CheckChargeAttackKey, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), true, -1.0f);
		}
	}
}

void ACPP_DarkLifeCharacter::SetRunSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void ACPP_DarkLifeCharacter::LookUp(float value)
{
	if (!bLockedEnemy) {
		if (bDrawingBow) {
			AddControllerPitchInput(value * 0.2f);
		}
		else AddControllerPitchInput(value);
	}
}

void ACPP_DarkLifeCharacter::Turn(float value)
{
	if (!bLockedEnemy) {
		if (bDrawingBow) {
			AddControllerYawInput(value * 0.2f);
		}
		else AddControllerYawInput(value);
	}
}

void ACPP_DarkLifeCharacter::Sprint()
{
	if (CurrentCharacterMovement != ECharacterMovement::Ladder) {
		if (!bBeastPowerMovement) {
			if (!bSprintKeyPress) {
				if (bCanThrowProjectile) {
					ThrowDeactivate();
					SetCharacterMovement(ECharacterMovement::Sprint);
				}
				SetCharacterMovement(ECharacterMovement::Sprint);
					
				
			}
		}
	}
}

// Called when the game starts or when spawned
void ACPP_DarkLifeCharacter::BeginPlay()
{
	Super::BeginPlay();
	bDrawSword = false;
	bDrawShield = false;
	bDrawSwordPreviousState = bDrawSword;
	bDrawShieldPreviousState = bDrawShield;
	bTorchPreviousState = false;
	SetCombatState(ECharacterCombatState::TwoBareHand);
	
}

void ACPP_DarkLifeCharacter::EvasionStepAnimations()
{
	if (Stamina >= 30.0) {
		if (GetInputAxisValue("MoveRight") > 0) {
			PlayAnimMontage(EvasionAnimations[0], EvasionSpeedValue);
	  }
		else {
			if (GetInputAxisValue("MoveRight") < 0) {
				PlayAnimMontage(EvasionAnimations[1], EvasionSpeedValue);
			}
			else {
				if (GetInputAxisValue("MoveForward") > 0) {
					PlayAnimMontage(EvasionAnimations[2], EvasionSpeedValue);
				}
				else if (GetInputAxisValue("MoveForward") < 0) {
					SpringArm->bEnableCameraLag = false;
					PlayAnimMontage(EvasionAnimations[3], EvasionSpeedValue);
					//SpringArm->bEnableCameraLag = true;
				}
			}
		}
	}

	if (!bBeastPowerMovement) {

		Stamina = UKismetMathLibrary::FClamp(Stamina - (StaminaSubstraction * 4.0), MaxStamina / StaminaDividerMinLimit, MaxStamina);
	}

}

void ACPP_DarkLifeCharacter::StaminaIncrease()
{
	if (bBlocking) {
		Stamina = UKismetMathLibrary::FClamp(Stamina + 0.15, 0.0, MaxStamina);
	}
	else {
		Stamina = UKismetMathLibrary::FClamp(Stamina + 2.0, 0.0, MaxStamina);
	}


	if (Stamina == MaxStamina) {
		GetWorldTimerManager().PauseTimer(StaminaIncreaseHandle);
	}
}

void ACPP_DarkLifeCharacter::StaminaDecrease()
{
	if (!bStaminaBoost) {
		Stamina = UKismetMathLibrary::FClamp(Stamina + (-1.0f), 0.0f, MaxStamina);
		if (Stamina <= 0.0f) {
			SetCharacterMovement(ECharacterMovement::Jog);
		}
	}
}

void ACPP_DarkLifeCharacter::PoisonIncrease()
{
	Health -= 0.2f;
}

void ACPP_DarkLifeCharacter::ResetCombo()
{
	bSaveAttack = false;
	bIsAttacking = false;
	GetWorldTimerManager().SetTimer(StaminaIncreaseHandle,this,&ACPP_DarkLifeCharacter::StaminaIncrease, StaminaIncreaseTime, true, StaminaIncreaseDelay);
	
}

void ACPP_DarkLifeCharacter::StopSprint()
{
	if (!bSlowRun) {
		bSprintKeyPress = false;
		if (bBeastPowerMovement) {
			GetCharacterMovement()->MaxWalkSpeed = BeastPowerSpeed;
		}
		else {
			if (bWalk) {
				if(bCrouched){GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;}
				else {GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;}
			}

			else if (bSlowRun)
			{
				if(bCrouched){GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;}
				else {GetCharacterMovement()->MaxWalkSpeed = RunSlowSpeed;}
				
			}

			else {
				if(bCrouched){GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;}
				else {GetCharacterMovement()->MaxWalkSpeed = RunSpeed;}
			}
		}
	}

		bSprint = false;

		if (StaminaDecreaseHandle.IsValid()) {
			GetWorldTimerManager().PauseTimer(StaminaDecreaseHandle);
			GetWorldTimerManager().ClearTimer(StaminaDecreaseHandle);
		}

		if (bLockedEnemy) {
			SpringArm->CameraLagSpeed = 30.0f;
		}

		GetWorldTimerManager().SetTimer(StaminaIncreaseHandle, this, &ACPP_DarkLifeCharacter::StaminaIncrease, StaminaIncreaseTime, true, 1.0f);
	
}

void ACPP_DarkLifeCharacter::StartSprint()
{
	bSprintKeyPress = true;
	if (( GetInputAxisValue("MoveForward")>0)&&(Stamina > 0.0f)&&(!bSlowRun)) {
		GetCharacterMovement()->MaxWalkSpeed = 700.0f;
		bSprint = true;
		SpringArm->bEnableCameraLag = true;
		SpringArm->CameraLagSpeed = 15.0f;

		if (StaminaIncreaseHandle.IsValid()) {
			GetWorldTimerManager().PauseTimer(StaminaIncreaseHandle);
			GetWorldTimerManager().ClearTimer(StaminaIncreaseHandle);
		}

		GetWorldTimerManager().SetTimer(StaminaDecreaseHandle, this, &ACPP_DarkLifeCharacter::StaminaDecrease, StaminaIncreaseTime, true, 0.0f);

	 }
	else {
		bSprintKeyPress = false;
	}
}

void ACPP_DarkLifeCharacter::SetCharacterMovement(ECharacterMovement NewMovement)
{
	CurrentCharacterMovement = NewMovement;
	CharacterMovementChange.Broadcast(NewMovement);
	switch (CurrentCharacterMovement)
	{
	case ECharacterMovement::Walk:
		StopSprint();
		SetWalkSpeed();
		bWalk = true;
		bCrouched = false;
		if (GetCharacterMovement()->IsCrouching()) {
			UnCrouch();
		}
		break;
	case ECharacterMovement::Jog:
		StopSprint();
		SetRunSpeed();
		bWalk = false;
		bCrouched = false;
		if (GetCharacterMovement()->IsCrouching()) {
			UnCrouch();
		}
		break;
	case ECharacterMovement::Sprint:
		StopAnimMontage(GetCurrentMontage());
		ResetCombo();
		StartSprint();
		bWalk = false;
		break;
	case ECharacterMovement::Crouch:
		bCrouched = true;
		SetCrouchSpeed();
		StopSprint();
		Crouch();
		break;
	case ECharacterMovement::Dodge:
		StopSprint();
		break;
	case ECharacterMovement::Ladder:
		StopSprint();
		break;
	default:
		break;
	}
}

/*void ACPP_DarkLifeCharacter::StartSlowRun(double SurfaceDistance, double MinimumDistance, double SpeedDecreaseFactor)
{
	if (SurfaceDistance <= MinimumDistance ) {
		if (!bSlowRun) {
			bSlowRun = true;
			double SpeedRatio;
			SpeedRatio = RunSpeed / SurfaceDistance;
			const double newSpeed = RunSpeed - (SpeedRatio * SpeedDecreaseFactor);
			GetCharacterMovement()->MaxWalkSpeed = UKismetMathLibrary::FClamp(newSpeed, RunSlowSpeed, RunSpeed);
		}
	}
	else if (bSlowRun) {
			bSlowRun = false;
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
			StopSprint();
		}
	
}
*/

void ACPP_DarkLifeCharacter::ShootArrow()
{
	bDrawFinish = false;
	//PlayAnimMontage(DrawBowAnimation);
	PlayAnimMontage(DrawBowAnimation,1.0f,"Default");
	
	
}

void ACPP_DarkLifeCharacter::SetTorchActive(bool bActivate, bool bRestorePreviousState)
{
	if (!bRestorePreviousState) {
		
		bTorchActive = bActivate;
		bTorchPreviousState = bTorchActive;
		Torch->SetHiddenInGame(!bTorchActive, true);
		
	}
	else {
		bTorchActive = bTorchPreviousState;
		Torch->SetHiddenInGame(!bTorchActive, true);
	}

	
	
	ThrowDeactivate();
	//StopSprint();
	
	
}

void ACPP_DarkLifeCharacter::ThrowDeactivate()
{
	StopAnimMontage(GetCurrentMontage());
	bCanThrowProjectile = false;
	bDrawProjectile = false;
	IgnisBomb->SetVisibility(false, false);

}

void ACPP_DarkLifeCharacter::ArcheryDeactivate()
{
	
	bArchery = false;
	

}

void ACPP_DarkLifeCharacter::HealthIncrease(double value)
{
	
	Health = UKismetMathLibrary::FClamp(Health + value,0.0f,MaxHealth);
}

bool ACPP_DarkLifeCharacter::DetectHitFromTheBack(FHitResult ReceivedHit)
{
	
	FVector NormImpactNormal = ReceivedHit.ImpactNormal;
	FVector NormForwardVector = GetActorForwardVector();
	UKismetMathLibrary::Vector_Normalize(NormImpactNormal, 0.0001f);
	UKismetMathLibrary::Vector_Normalize(NormForwardVector, 0.0001f);
	double dotProduct = UKismetMathLibrary::Dot_VectorVector(NormImpactNormal, NormForwardVector);
	double AcosD = UKismetMathLibrary::DegAcos(dotProduct);
	return UKismetMathLibrary::InRange_FloatFloat(AcosD, 90.0f, 180.0f, true, true);

}

void ACPP_DarkLifeCharacter::UnlockTarget()
{
	bLockedEnemy = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	CurrentEnemy = nullptr;
	
}

void ACPP_DarkLifeCharacter::JumpActivation(bool ActivationValue)
{
	bJump = ActivationValue;
}

void ACPP_DarkLifeCharacter::PerformJump()
{
	if (bJump) {
		StopSprint();
		Jump(); 
	}
}

void ACPP_DarkLifeCharacter::HandWeaponsVisibility(bool hide)
{
	if (bTorchActive) {
		Torch->SetHiddenInGame(hide, true);
	}

	if (bDrawSword) {
		Excalibur->SetHiddenInGame(hide, true);
	}


	if (bDrawShield) {
		ShieldMesh->SetHiddenInGame(hide, true);
	}
}

void ACPP_DarkLifeCharacter::PlayAnimationByCharacterState(int32 animationIndex, bool& Success)
{
	Success = false;

	if (!bBlocking) {

		if (bSprint) {
			if (AttackOnSprintAnimations.IsValidIndex((int32)CombatState)) {
				PlayAnimMontage(AttackOnSprintAnimations[(int32)CombatState]);
				UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(GetWorld(), ChargeAttackTimer);
				Success = true;
			}
		}
		else {
			if ((!CurrentStateAnimations.IsEmpty()) && (CurrentStateAnimations.IsValidIndex(ComboCounter))) {
				if ((animationIndex + 1) == CurrentStateAnimations.Num()) {
					ComboCounter = 0;
					PlayAnimMontage(CurrentStateAnimations[animationIndex]);
					Attacking.Broadcast();
					Success = true;

				}
				else {
					ComboCounter = animationIndex + 1;
					PlayAnimMontage(CurrentStateAnimations[animationIndex]);
					Attacking.Broadcast();
					Success = true;

				}
			}
		}

		StopSprint();
		bBlocking = false;

	}


	else {
		PlayAnimMontage(ShieldAttackAnimations[0]);
		bBlocking = false;
		Success = true;
	}


	UpdateStaminaByCharacterCombatState();
	//StopSprint();

}

void ACPP_DarkLifeCharacter::SaveComboAttack()
{
	if (bSaveAttack) {
		bIsAttacking = true;
	}
}

void ACPP_DarkLifeCharacter::SetVariablesByCombatState()
{
	bDrawSwordPreviousState = bDrawSword;
	ComboCounter = 0;
	
	switch (CombatState)
	{
	case ECharacterCombatState::OneHandSword:
		bDrawSword = true;
		if (!bDrawShield) { 
			SetTorchActive(true, false); 
			
		}
		else { 
			SetTorchActive(false, false); 
			
		}
		CurrentStateAnimations = OneHandSwordAnimations;
		Fracture = UKismetMathLibrary::FClamp(Fracture - 10, 10.0f, 9999.0f);
		break;
	case ECharacterCombatState::TwoHandSword:
		bDrawSword = true;
		bDrawShieldPreviousState = bDrawShield;
		bDrawShield = false;
		bTorchPreviousState = bTorchActive;
		bTorchActive = false;
		SetTorchActive(false, bTorchPreviousState);
		Fracture = UKismetMathLibrary::FClamp(Fracture + 10, Fracture, 9999.0f);
		CurrentStateAnimations = TwoHandSwordAnimations;
		break;
	case ECharacterCombatState::OneHandShield:
		bDrawShield = true;
		bDrawSword = false;
		SetTorchActive(false, false);
		//CurrentStateAnimations = ShieldAttackAnimations;
		//Just for testing
		CurrentStateAnimations = BareHandAttackAnimations;
		break;
	case ECharacterCombatState::OneHandTorch:
		bDrawSword = false;
		bDrawShield = false;
		SetTorchActive(true, false);
		//CurrentStateAnimations = TorchAttackAnimations;
		//Just for testing
		CurrentStateAnimations = BareHandAttackAnimations;
		break;
	case ECharacterCombatState::TwoBareHand:
		bDrawSword = false;
		bDrawShield = false;
		SetTorchActive(false, false);
		CurrentStateAnimations = BareHandAttackAnimations;
		break;
	default:
		break;
	}
}

void ACPP_DarkLifeCharacter::SetCombatState(ECharacterCombatState newCombatState)
{
	PreviousCombatState = CombatState;
	CombatState = newCombatState;
	SetVariablesByCombatState();
}

void ACPP_DarkLifeCharacter::HitAnimation(FHitResult HitInfo, EEnemyDamageType DamageType)
{
	FVector ImpactNormal = HitInfo.ImpactNormal;
	switch (DamageType)
	{
	case EEnemyDamageType::RegularDamage:
		PlayRegularDamageHitAnimation(ImpactNormal);
		break;
	case EEnemyDamageType::StrongDamage:
		break;
	case EEnemyDamageType::StuntDamage:
		PlayStuntDamageHitAnimation();
		break;
	default:
		break;
	}

}

void ACPP_DarkLifeCharacter::PlayRegularDamageHitAnimation(FVector ImpactNormal)
{
	if (HitAnimations.Num() > 1) {
		if (HitAnimations.IsValidIndex(0) && (HitAngleInRange(ImpactNormal, GetActorForwardVector(), 100.0f, 180.0f, true, true))) {
			//Back Hit Animation
			PlayAnimMontage(HitAnimations[3]);
		}
		else if (HitAnimations.IsValidIndex(1) && (HitAngleInRange(ImpactNormal, GetActorForwardVector(), 54.0f, 90.0f, true, true))) {

			if (HitAngleInRange(ImpactNormal, GetActorRightVector(), 0.0f, 90.0f, true, false)) {
				//Right Hit Animation
				PlayAnimMontage(HitAnimations[2]);
			}
			else {
				//Left Hit Animation
				PlayAnimMontage(HitAnimations[1]);
			}
		}
		else if (HitAnimations.IsValidIndex(2) && (HitAngleInRange(ImpactNormal, GetActorForwardVector(), 0.0f, 44.0f, true, true)))
		{
			//Front Hit Animation
			PlayAnimMontage(HitAnimations[0]);
		}
	}
	else {

		//If only have one hit animation or Front Hit Animation
		if (HitAnimations.IsValidIndex(0)) {
			PlayAnimMontage(HitAnimations[0]);
		}
	}
}

void ACPP_DarkLifeCharacter::PlayStuntDamageHitAnimation()
{
	if ((CombatState == ECharacterCombatState::OneHandSword) || (CombatState == ECharacterCombatState::OneHandShield))
	{
		if (StuntAnimations.IsValidIndex(0)) {
			PlayAnimMontage(StuntAnimations[0]);
		}
	}
}

void ACPP_DarkLifeCharacter::SetCharacterNegativeStatus(ECharacterNegativeStatus NewNegativeStatus)
{
	CurrentCharacterNegativeStatus = NewNegativeStatus;
}

void ACPP_DarkLifeCharacter::PlayParryFinisherAnimation(int parryIndex, UAnimMontage*& ParryMontage)
{
	if (ParryFinisher.IsValidIndex(parryIndex)) {
		if (IsValid(ParryFinisher[parryIndex])) {
			PlayAnimMontage(ParryFinisher[parryIndex]);
			ParryMontage = ParryFinisher[parryIndex];
			
		}
	}
}

void ACPP_DarkLifeCharacter::PlayFromTheBackFinisherAnimation(int backFinishIndex, UAnimMontage*& FromTheBackMontage)
{
	if (BackFinisher.IsValidIndex(backFinishIndex)) {
		if (IsValid(BackFinisher[backFinishIndex])) {
			PlayAnimMontage(BackFinisher[backFinishIndex]);
			FromTheBackMontage = BackFinisher[backFinishIndex];
		}
	 }
}

void ACPP_DarkLifeCharacter::PlayRandomFinishAnimation(ECharacterFinishMoveType FinishMovementType, double& AnimationLength)
{
	if (IsValid(CurrentEnemy)) {
		ACPP_Enemy* EnemyRef = Cast<ACPP_Enemy>(CurrentEnemy);
		if (IsValid(EnemyRef)) {
			if ((EnemyRef->bCanBeExecuted)&&(EnemyRef->bExecutionActive)) {

				int randomIndex = 0;
				UAnimMontage* ParryMontage = nullptr;
				UAnimMontage* BackMontage = nullptr;

				switch (FinishMovementType)
				{
				case ECharacterFinishMoveType::AfterParry:
					
					randomIndex = UKismetMathLibrary::RandomInteger(ParryFinisher.Num());
					PlayParryFinisherAnimation(randomIndex, ParryMontage);
					EnemyRef->PlayParryFinisherAnimation(randomIndex);
					AnimationLength = ParryMontage->GetPlayLength();
					break;
				case ECharacterFinishMoveType::FromTheBack:
					
					randomIndex = UKismetMathLibrary::RandomInteger(BackFinisher.Num());
					PlayFromTheBackFinisherAnimation(randomIndex, BackMontage);
					EnemyRef->PlayFromTheBackFinisherAnimation(randomIndex);
					AnimationLength = BackMontage->GetPlayLength();
					break;
				default:
					
					randomIndex = UKismetMathLibrary::RandomInteger(BackFinisher.Num());
					PlayFromTheBackFinisherAnimation(randomIndex, BackMontage);
					EnemyRef->PlayFromTheBackFinisherAnimation(randomIndex);
					AnimationLength = BackMontage->GetPlayLength();
					break;
				}
		  }
		}
	}
}

void ACPP_DarkLifeCharacter::PlayRandomChargeAnimationByCombatState()
{ 
	int AnimationsCount;
	TArray<UAnimMontage*> ChargeAttackArray;

	switch (CombatState)
	{
	case ECharacterCombatState::OneHandSword:
		AnimationsCount = UKismetMathLibrary::RandomInteger(OneHandChargeAttack.Num());
		ChargeAttackArray = OneHandChargeAttack;
		break;
	case ECharacterCombatState::TwoHandSword:
		AnimationsCount = UKismetMathLibrary::RandomInteger(TwoHandsChargeAttack.Num());
		ChargeAttackArray = TwoHandsChargeAttack;
		break;
	case ECharacterCombatState::OneHandShield:
		break;
	case ECharacterCombatState::OneHandTorch:
		break;
	case ECharacterCombatState::TwoBareHand:
		break;
	default:
		break;
	}

	if (ChargeAttackArray.Num() > 0) {
		AnimationsCount = UKismetMathLibrary::RandomInteger(ChargeAttackArray.Num());
		if (ChargeAttackArray.IsValidIndex(AnimationsCount)) {
			PlayAnimMontage(ChargeAttackArray[AnimationsCount]);
			ResetCombo();
		}

	}

}



// Called every frame
void ACPP_DarkLifeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACPP_DarkLifeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACPP_DarkLifeCharacter::PerformJump);
	PlayerInputComponent->BindAxis("LookUp", this, &ACPP_DarkLifeCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &ACPP_DarkLifeCharacter::Turn);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, & ACPP_DarkLifeCharacter::Sprint);

}




