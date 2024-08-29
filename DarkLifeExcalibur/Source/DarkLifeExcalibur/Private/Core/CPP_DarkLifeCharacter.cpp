// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/CPP_DarkLifeCharacter.h"
#include "Core/CPP_Enemy.h"
#include "UObject/UnrealType.h"
#include "UObject/PropertyPortFlags.h"
#include "Core/CPP_GameInstance.h"
#include "Core/Components/CPP_ItemContainer.h"


// Sets default values
ACPP_DarkLifeCharacter::ACPP_DarkLifeCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	
	//Parameters Default Values
	Health = 100;
	MaxHealth = 100;
	Stamina = 300;
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
	
	InventoryManager = CreateDefaultSubobject<UCPP_ItemContainer>(TEXT("InventoryManager"));

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
	RunSpeed = 300.0;
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
	CrouchSpeed = 180.0;
	
		
}

void ACPP_DarkLifeCharacter::UpdateStaminaByCharacterCombatState()
{
	switch (CombatState)
	{
	case ECharacterCombatState::OneHandSword:
		Stamina = UKismetMathLibrary::FClamp(Stamina - 10.0, 0.0, MaxStamina);
		break;
	case ECharacterCombatState::TwoHandSword:
		Stamina = UKismetMathLibrary::FClamp(Stamina - 10.0, 0.0, MaxStamina);
		break;
	case ECharacterCombatState::OneHandShield:
		break;
	case ECharacterCombatState::OneHandTorch:
		break;
	case ECharacterCombatState::TwoBareHand:
		Stamina = UKismetMathLibrary::FClamp(Stamina -  12.0, 0.0, MaxStamina);
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
		UKismetSystemLibrary::K2_PauseTimer(this, "ResetComboCounter");
		UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(this, ResetComboCounterHandle);

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

void ACPP_DarkLifeCharacter::ResetComboCounter()
{
	ComboCounter = 0;
}

void ACPP_DarkLifeCharacter::SetRunSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void ACPP_DarkLifeCharacter::LookUp(float value)
{
	
		if (bDrawingBow) {
			AddControllerPitchInput(value * 0.2f);
		}
	
		else AddControllerPitchInput(value);
	
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
	if ((CurrentCharacterMovement != ECharacterMovement::Ladder) && (CurrentCharacterState == ECharacterState::Normal)) {
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


void ACPP_DarkLifeCharacter::InitializeCharacter_Implementation()
{
	
	UCPP_GameInstance* DLGameInstance = Cast<UCPP_GameInstance>(GetGameInstance());
	if (!DLGameInstance)
	{
		
		return;
	}

	
	FProperty* SaveGameProperty = DLGameInstance->GetClass()->FindPropertyByName(FName(TEXT("Save Game")));
	if (!SaveGameProperty)
	{
		return;
	}

	void* SaveGamePropertyValue = SaveGameProperty->ContainerPtrToValuePtr<void>(DLGameInstance);
	if (!SaveGamePropertyValue)
	{	
		return;
	}

	UCPP_DarkLifeSaveGame* SaveGame = *reinterpret_cast<UCPP_DarkLifeSaveGame**>(SaveGamePropertyValue);
	if (!SaveGame)
	{
		return;
	}

	SetCharacterState(SaveGame->CharacterState);
	if (InventoryManager)
	{
		InventoryManager->LoadSavedInfo(SaveGame);
	}


	if (CurrentCharacterState == ECharacterState::Injuried_Sword)
	{
		bool retFlag;
		SetFakeExcalibur(retFlag);
		if (retFlag) return;

	}
	else if (CurrentCharacterState == ECharacterState::Normal) {
		SetExcalibur();
		SetBow();
		SetShield();
	}
	else if (CurrentCharacterState == ECharacterState::Injuried) {
		HideWeapons(true);
	}
	else
	{
		if (FakeExcaliburActorRef)
		{
			FakeExcaliburActorRef->Destroy();
			FakeExcaliburActorRef = nullptr;
		}
	}



}



void ACPP_DarkLifeCharacter::LoadParameters()
{
	UCPP_GameInstance* DLGameInstance = Cast<UCPP_GameInstance>(GetGameInstance());
	UCPP_DarkLifeSaveGame* SaveGame = DLGameInstance->SaveGameObject;
	if (SaveGame) {
		SaveGame->ParametersCalculation();
		Health, MaxHealth = SaveGame->CurrentHealth;
		Stamina, MaxStamina = SaveGame->CurrentStamina;
		Fracture = SaveGame->CurrentFracture;
		Defense = SaveGame->CurrentDefense;
		Recharge = SaveGame->CurrentRecharge;
		MaxBeast = SaveGame->CurrentBeast;
		
	}
}

void ACPP_DarkLifeCharacter::SetExcalibur()
{
	// Comprobación de puntero nulo para Excalibur
	if (Excalibur)
	{
		// Si Excalibur tiene un ChildActor, destrúyelo
		if (Excalibur->GetChildActor())
		{
			Excalibur->GetChildActor()->Destroy();
		}

		// Cargar la clase del Blueprint de Excalibur
		UClass* ExcaliburClass = StaticLoadClass(AActor::StaticClass(), nullptr, TEXT("/Game/TESTING/Character/Excalibur/Modular_Fantasy_Sword/Blueprints/BP_DarkLifeModularSword.BP_DarkLifeModularSword_C"));
		if (ExcaliburClass)
		{
			Excalibur->SetChildActorClass(ExcaliburClass);
			LoadParameters();
		}
		
	}
	
}

void ACPP_DarkLifeCharacter::SetBow()
{
	// Comprobación de puntero nulo para LongBow
	if (LongBow)
	{
		// Si LongBow tiene un ChildActor, destrúyelo
		if (LongBow->GetChildActor())
		{
			LongBow->GetChildActor()->Destroy();
		}

		// Cargar la clase del Blueprint del Arco
		UClass* BowClass = StaticLoadClass(AActor::StaticClass(), nullptr, TEXT("/Game/TESTING/Character/Bow/BP_Bow.BP_Bow_C"));
		if (BowClass)
		{
			LongBow->SetChildActorClass(BowClass);
		}
		
	}
	
}

void ACPP_DarkLifeCharacter::SetShield()
{
	// Comprobar si InventoryManager y Shield existen
	if (InventoryManager && InventoryManager->Shield)
	{
		TSoftObjectPtr<UStaticMesh> ShieldSoftReference = InventoryManager->Shield->EquipMesh;

		// Comprobar si ShieldMesh existe
		if (ShieldMesh)
		{
			// Comprobar si ShieldSoftReference es válido
			if (ShieldSoftReference.IsValid())
			{
				ShieldMesh->SetStaticMesh(ShieldSoftReference.Get());
			}
			else
			{
				// Cargar sincrónicamente el recurso si no es válido
				ShieldSoftReference.LoadSynchronous();
				if (ShieldSoftReference.IsValid())
				{
					ShieldMesh->SetStaticMesh(ShieldSoftReference.Get());
				}
				
			}
		}
		
	}
	
}

void ACPP_DarkLifeCharacter::EvasionStepAnimations()
{

	ECharacterInputDirection DodgeDirection = ECharacterInputDirection::None;
	if ((GetInputAxisValue("MoveForward") > 0) && (GetInputAxisValue("MoveRight") == 0)) {
		DodgeDirection = ECharacterInputDirection::Forward;
	}
	else if ((GetInputAxisValue("MoveForward") > 0) && (GetInputAxisValue("MoveRight") > 0)) {
		DodgeDirection = ECharacterInputDirection::ForwardRight;
	}
	else if ((GetInputAxisValue("MoveForward") == 0) && (GetInputAxisValue("MoveRight") > 0)) {
		DodgeDirection = ECharacterInputDirection::Right;
	}
	else if ((GetInputAxisValue("MoveForward") < 0) && (GetInputAxisValue("MoveRight") > 0)) {
		DodgeDirection = ECharacterInputDirection::BackwardRight;
	}
	else if ((GetInputAxisValue("MoveForward") < 0) && (GetInputAxisValue("MoveRight") == 0)) {
		DodgeDirection = ECharacterInputDirection::Backward;
	}
	else if ((GetInputAxisValue("MoveForward") < 0) && (GetInputAxisValue("MoveRight") < 0)) {
		DodgeDirection = ECharacterInputDirection::BackwardLeft;
	}
	else if ((GetInputAxisValue("MoveForward") == 0) && (GetInputAxisValue("MoveRight") < 0)) {
		DodgeDirection = ECharacterInputDirection::Left;
	}
	else if ((GetInputAxisValue("MoveForward") > 0) && (GetInputAxisValue("MoveRight") < 0)) {
		DodgeDirection = ECharacterInputDirection::ForwardLeft;
	}
	else if ((GetInputAxisValue("MoveForward") == 0) && (GetInputAxisValue("MoveRight") == 0)) {
		DodgeDirection = ECharacterInputDirection::None;
	}



	if (Stamina >= 30.0) {
		PlayAnimMontage(EvasionAnimations[(int8)DodgeDirection], EvasionSpeedValue);
	}

	if (!bBeastPowerMovement) {

		Stamina = UKismetMathLibrary::FClamp(Stamina - 10.0, MaxStamina / StaminaDividerMinLimit, MaxStamina);
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
		Stamina = UKismetMathLibrary::FClamp(Stamina + (-0.5f), 0.0f, MaxStamina);
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
	
	//Reset Combo Counter after 1.5 sec no loop
	GetWorldTimerManager().SetTimer(ResetComboCounterHandle, this, &ACPP_DarkLifeCharacter::ResetComboCounter, 1.5f, false , 1.5f);
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
		if (GetCharacterMovement()->IsCrouching()) {
			UnCrouch();
		}
		StartSprint();
		bWalk = false;
		bCrouched = false;
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
	case ECharacterMovement::Helping:
		SetWalkSpeed();
		bWalk = true;
		StopSprint();
		ResetCombo();
		if (GetCharacterMovement()->IsCrouching()) {
			UnCrouch();
		}
		break;
	case ECharacterMovement::Injuried:
		SetWalkSpeed();
		bWalk = true;
		StopSprint();
		if (GetCharacterMovement()->IsCrouching()) {
			UnCrouch();
		}
		break;
	case ECharacterMovement::Injuried_Sword:
		SetWalkSpeed();
		bWalk = true;
		StopSprint();
		if (GetCharacterMovement()->IsCrouching()) {
			UnCrouch();
		}
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
	if ((bJump)&&(CurrentCharacterState == ECharacterState::Normal)) {
		bJump = false;
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

void ACPP_DarkLifeCharacter::PlayParryFinisherAnimation(int parryIndex, UAnimMontage*& ParryMontage, TArray<UAnimMontage*>& ParryAnimList)
{
	if (ParryAnimList.IsValidIndex(parryIndex)) {
		if (IsValid(ParryAnimList[parryIndex])) {
			PlayAnimMontage(ParryAnimList[parryIndex]);
			ParryMontage = ParryAnimList[parryIndex];
			
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

void ACPP_DarkLifeCharacter::PlayRandomFinishAnimation(ECharacterFinishMoveType FinishMovementType, double& AnimationLength, bool& Success)
{
	Success = true;
	if (IsValid(CurrentEnemy)) {
		ACPP_Enemy* EnemyRef = Cast<ACPP_Enemy>(CurrentEnemy);
		if (IsValid(EnemyRef)) {
			if ((EnemyRef->bCanBeExecuted) && (EnemyRef->bExecutionActive)) {

				int randomIndex = 0;
				UAnimMontage* ParryMontage = nullptr;
				UAnimMontage* BackMontage = nullptr;
				TArray<UAnimMontage*> LocalParryFinisherAnimation;

				switch (FinishMovementType)
				{
				case ECharacterFinishMoveType::AfterParry:

					if (CombatState == ECharacterCombatState::TwoHandSword) {
						LocalParryFinisherAnimation = TwoHandsParryFinisher;
					}
					else LocalParryFinisherAnimation = OneHandParryFinisher;

					if (!LocalParryFinisherAnimation.IsEmpty()) {

						randomIndex = UKismetMathLibrary::RandomInteger(LocalParryFinisherAnimation.Num());
						UAIBlueprintHelperLibrary::GetAIController(EnemyRef)->GetBrainComponent()->PauseLogic("Finisher Received");
						EnemyRef->GetMesh()->Stop();
						PlayParryFinisherAnimation(randomIndex, ParryMontage, LocalParryFinisherAnimation);
						EnemyRef->PlayParryFinisherAnimation(randomIndex, CombatState);
						AnimationLength = ParryMontage->GetPlayLength();
						Success = true;
						ResetCombo();
						break;
					}

					else {
						Success = false;
						break;
					}
				case ECharacterFinishMoveType::FromTheBack:

					if (!BackFinisher.IsEmpty()) {

						randomIndex = UKismetMathLibrary::RandomInteger(BackFinisher.Num());
						EnemyRef->GetMesh()->Stop();
						UAIBlueprintHelperLibrary::GetAIController(EnemyRef)->GetBrainComponent()->PauseLogic("Finisher Received");
						PlayFromTheBackFinisherAnimation(randomIndex, BackMontage);
						EnemyRef->PlayFromTheBackFinisherAnimation(randomIndex);
						AnimationLength = BackMontage->GetPlayLength();
						Success = true;
						ResetCombo();
						break;
					}
					else {
						Success = false;
						break;
					}

				default:

					randomIndex = UKismetMathLibrary::RandomInteger(BackFinisher.Num());
					PlayFromTheBackFinisherAnimation(randomIndex, BackMontage);
					EnemyRef->PlayFromTheBackFinisherAnimation(randomIndex);
					AnimationLength = BackMontage->GetPlayLength();
					Success = true;
					ResetCombo();
					break;
				}
			}
		}

		else Success = false;
	}

	else Success = false;
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

void ACPP_DarkLifeCharacter::EnemyAttacking(ACPP_Enemy* Enemy)
{
	bPlayerIsEngaged = true;
	EnemyAttackingRef = Enemy;
	CharacterEngaged.Broadcast(true, Enemy);
	
}

void ACPP_DarkLifeCharacter::HideWeapons(bool bHide)
{
	Excalibur->bHiddenInGame = bHide;
	Excalibur->SetVisibility(!bHide, true);
	ShieldMesh->bHiddenInGame = bHide;
	ShieldMesh->SetVisibility(!bHide, true);
	LongBow->bHiddenInGame = bHide;
	LongBow->SetVisibility(!bHide, true);
	Torch->bHiddenInGame = bHide;
	Torch->SetVisibility(!bHide, true);
}

void ACPP_DarkLifeCharacter::PlayDeflectedAnimation(int CustomComboIndex, ECharacterDamageType DamageType)
{
	switch (DamageType)
	{
	case ECharacterDamageType::Sword:
		if (CombatState == ECharacterCombatState::OneHandSword) {
			if (OneHandDeflectedAnimations.IsValidIndex(CustomComboIndex) && (IsValid(OneHandDeflectedAnimations[CustomComboIndex]))) {
				PlayAnimMontage(OneHandDeflectedAnimations[CustomComboIndex]);
			}
		}
		else if (CombatState == ECharacterCombatState::TwoHandSword) {
			if (TwoHandsDeflectedAnimations.IsValidIndex(CustomComboIndex) && (IsValid(TwoHandsDeflectedAnimations[CustomComboIndex]))) {
				PlayAnimMontage(TwoHandsDeflectedAnimations[CustomComboIndex]);
			}
		}
		break;
	case ECharacterDamageType::Shield:
		if (ShieldDeflectedAnimations.IsValidIndex(CustomComboIndex) && (IsValid(ShieldDeflectedAnimations[CustomComboIndex])))
		{
			PlayAnimMontage(ShieldDeflectedAnimations[CustomComboIndex]);
     	}
		break;
	case ECharacterDamageType::Torch:
		break;
	case ECharacterDamageType::Punch:
		if (PunchDeflectedAnimations.IsValidIndex(CustomComboIndex) && (IsValid(PunchDeflectedAnimations[CustomComboIndex])))
		{
			PlayAnimMontage(PunchDeflectedAnimations[CustomComboIndex]);
		}
		break;
	case ECharacterDamageType::Arrow:
		break;
	default:
		break;
	}

	
	
}

void ACPP_DarkLifeCharacter::PlayBlockingAnimations()
{
	if (BlockAnimations.Find(CombatState)&&(IsValid(BlockAnimations[CombatState]))) {
		PlayAnimMontage(BlockAnimations[CombatState]);
	}
}

void ACPP_DarkLifeCharacter::PlayBlockingHitAnimations()
{
	if (BlockHitAnimations.Find(CombatState) && (IsValid(BlockHitAnimations[CombatState]))) {
		PlayAnimMontage(BlockHitAnimations[CombatState]);
	}
}

void ACPP_DarkLifeCharacter::SetCharacterState(ECharacterState NewCharacterstate)
{
	CurrentCharacterState = NewCharacterstate;

	switch (CurrentCharacterState) {
	case ECharacterState::Normal:
		SetCharacterMovement(ECharacterMovement::Jog);
		break;
	case ECharacterState::Helping:
		SetCharacterMovement(ECharacterMovement::Walk);
		break;
	case ECharacterState::Injuried:
		SetCharacterMovement(ECharacterMovement::Injuried);
		break;
	case ECharacterState::Injuried_Sword:
		SetCharacterMovement(ECharacterMovement::Injuried_Sword);
		bool retFlag;
		SetFakeExcalibur(retFlag);
		if (retFlag) return;
		break;
	default:
		break;
	}

	UCPP_GameInstance* DLGameInstance = Cast<UCPP_GameInstance>(GetGameInstance());
	if (DLGameInstance) {
		DLGameInstance->SaveGame();
	}

}

void ACPP_DarkLifeCharacter::SetFakeExcalibur(bool& retFlag)
{
	retFlag = true;
	static const FString FakeExcaliburPath = TEXT("/Game/TESTING/Character/Excalibur/Modular_Fantasy_Sword/Blueprints/BP_FakeExcalibur.BP_FakeExcalibur_C");
	UClass* FakeExcaliburClass = StaticLoadClass(AActor::StaticClass(), nullptr, *FakeExcaliburPath);

	if (!FakeExcaliburClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	FakeExcaliburActorRef = GetWorld()->SpawnActor<AActor>(FakeExcaliburClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);

	if (FakeExcaliburActorRef)
	{
		USkeletalMeshComponent* FakeExcaliburMesh = GetMesh();
		if (FakeExcaliburMesh)
		{
			if (FakeExcaliburMesh->DoesSocketExist(TEXT("Sword")))
			{
				FakeExcaliburActorRef->AttachToComponent(FakeExcaliburMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("Sword"));

				FVector Location(2.0f, 2.0f, 7.0f);
				FRotator Rotation(-11.0f, 15.0f, 20.0f);
				FVector Scale(1.0f, 1.0f, 1.0f);

				FTransform FExcaliburTransform(Rotation, Location, Scale);
				FakeExcaliburActorRef->SetActorRelativeTransform(FExcaliburTransform);
			}
		}
	}
	retFlag = false;
}

void ACPP_DarkLifeCharacter::CharacterDrawSword(bool bOnlyToBack)
{
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true);
	if ((bDrawSword)||(bOnlyToBack)) {

		Excalibur->AttachToComponent(GetMesh(), AttachmentRules, "Sword_Back");

		if (bDrawShield) {
			SetCombatState(ECharacterCombatState::OneHandShield);
		}
		else if (bTorchActive) {
			SetCombatState(ECharacterCombatState::OneHandTorch);
		}
		else {
			SetCombatState(ECharacterCombatState::TwoBareHand);
		}
	}

	else {
		Excalibur->AttachToComponent(GetMesh(), AttachmentRules, "Sword");
		if ((bDrawShield) || (bTorchActive)) {

			SetCombatState(ECharacterCombatState::OneHandSword);
		}
		else {
			SetCombatState(ECharacterCombatState::TwoHandSword);
		}


		ResetCombo();
		//PlayerCharacter->StopSprint();
	}
}

void ACPP_DarkLifeCharacter::CharacterDrawShield(bool bOnlyToBack)
{
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true);
	if ((bDrawShield) || (bOnlyToBack)) {
		ShieldMesh->AttachToComponent(GetMesh(), AttachmentRules, "Shield_Back");
		if (bDrawSword) {

			SetCombatState(ECharacterCombatState::TwoHandSword);
		}

		else {
			SetCombatState(ECharacterCombatState::TwoBareHand);
		}
	}
	else {
		ShieldMesh->AttachToComponent(GetMesh(), AttachmentRules, "Shield");
		if (bDrawSword) {
			bDrawShield = true;
			SetCombatState(ECharacterCombatState::OneHandSword);
		}
		else {
			SetCombatState(ECharacterCombatState::OneHandShield);
		}
	}

	ResetCombo();
	//PlayerCharacter->StopSprint();
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




