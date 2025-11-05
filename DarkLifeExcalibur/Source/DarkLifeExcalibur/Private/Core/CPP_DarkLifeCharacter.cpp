// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/CPP_DarkLifeCharacter.h"

#include "StaticMeshAttributes.h"
#include "Core/CPP_Enemy.h"
#include "UObject/UnrealType.h"
#include "UObject/PropertyPortFlags.h"
#include "Core/CPP_GameInstance.h"
#include "Core/Components/CPP_ItemContainer.h"
#include "Data/Items/CPP_DA_Item_Heal.h"
#include "Widgets/Text/ISlateEditableTextWidget.h"


// Sets default values
ACPP_DarkLifeCharacter::ACPP_DarkLifeCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Components Init
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->SetRelativeLocation(FVector(-12.0f, 0.0f, 188.0f));
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

	AbilityComponent = CreateDefaultSubobject<UCPP_DarkLifeGASComponent>(TEXT("AbilityComponent"));
}

void ACPP_DarkLifeCharacter::UpdateStaminaByCharacterCombatState()
{
	double OneHandStaminaDecrease = 0.5;
	double TwoHandsStaminaDecrease = 0.5;
	double BareHandsStaminaDecrease = 0.5;
	if(CharacterParams)
	{
		OneHandStaminaDecrease = CharacterParams->OneHandAttackStaminaDecrease;
		TwoHandsStaminaDecrease = CharacterParams->TwoHandsAttackStaminaDecrease;
		BareHandsStaminaDecrease = CharacterParams->BareHandsStaminaDecrease;
	}
	switch (CombatState)
	{
	case ECharacterCombatState::OneHandSword:
		Stamina = UKismetMathLibrary::FClamp(Stamina - OneHandStaminaDecrease, 0.0, MaxStamina);
		break;
	case ECharacterCombatState::TwoHandSword:
		Stamina = UKismetMathLibrary::FClamp(Stamina - TwoHandsStaminaDecrease, 0.0, MaxStamina);
		break;
	case ECharacterCombatState::OneHandShield:
		break;
	case ECharacterCombatState::OneHandTorch:
		break;
	case ECharacterCombatState::TwoBareHand:
		Stamina = UKismetMathLibrary::FClamp(Stamina - BareHandsStaminaDecrease, 0.0, MaxStamina);
		break;
	default:
		break;
	}
}

bool ACPP_DarkLifeCharacter::HitAngleInRange(FVector ImpactNormal, FVector Vector, double MinAngle, double MaxAngle,
                                             bool bInclusiveMin, bool bInclusiveMax)
{
	double dotProduct = UKismetMathLibrary::Dot_VectorVector(ImpactNormal, Vector);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::SanitizeFloat(UKismetMathLibrary::DegAcos(dotProduct)));
	return UKismetMathLibrary::InRange_FloatFloat(UKismetMathLibrary::DegAcos(dotProduct), MinAngle, MaxAngle,
	                                              bInclusiveMin, bInclusiveMax);
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
	bool bKeyDownTimeCheck = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetInputKeyTimeDown(LightAttackKey)
		>= ChargeAttackKeyDownTime;
	if ((UKismetInputLibrary::Key_IsValid(LightAttackKey)) && bKeyDownTimeCheck)
	{
		if (!bIsAttacking)
		{
			PlayRandomChargeAnimationByCombatState();
			UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(GetWorld(), ChargeAttackTimer);
		}
		else
		{
			UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(GetWorld(), ChargeAttackTimer);
		}
	}
}

void ACPP_DarkLifeCharacter::AttackFunction()
{
	//if (Stamina >= 20.0f) {

	UKismetSystemLibrary::K2_PauseTimer(this, "StaminaIncrease");
	UKismetSystemLibrary::K2_PauseTimer(this, "ResetComboCounter");
	UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(this, ResetComboCounterHandle);

	if (bIsAttacking)
	{
		bSaveAttack = true;
	}
	else
	{
		bIsAttacking = true;
		bAttackKeyPressed = true;
		bool bAttackSuccess;
		PlayAnimationByCharacterState(ComboCounter, bAttackSuccess);
		SetCharacterMovement(ECharacterMovement::Jog);
		GetWorldTimerManager().SetTimer(ChargeAttackTimer, this, &ACPP_DarkLifeCharacter::CheckChargeAttackKey,
		                                UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), true, -1.0f);
	}
	//}
}

void ACPP_DarkLifeCharacter::ResetComboCounter()
{
	ComboCounter = 0;
}

void ACPP_DarkLifeCharacter::SaveMainInventory()
{
	UCPP_GameInstance* GameInstanceRef = Cast<UCPP_GameInstance>(GetGameInstance());
	if (GameInstanceRef)
	{
		FProperty* SaveGameProperty = GameInstanceRef->GetClass()->FindPropertyByName(FName(TEXT("Save Game")));
		void* SaveGamePropertyValue = SaveGameProperty->ContainerPtrToValuePtr<void>(GameInstanceRef);
		UCPP_DarkLifeSaveGame* SaveGame = *reinterpret_cast<UCPP_DarkLifeSaveGame**>(SaveGamePropertyValue);
		if (SaveGame)
		{
			SaveGame->Inventory = InventoryManager->MainInventory;
			GameInstanceRef->SaveGame();
		}
	}
}

void ACPP_DarkLifeCharacter::SetRunSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void ACPP_DarkLifeCharacter::LookUp(float value)
{
	if (bDrawingBow)
	{
		AddControllerPitchInput(value * 0.2f);
	}

	else AddControllerPitchInput(value);
}

void ACPP_DarkLifeCharacter::Turn(float value)
{
	if (!bLockedEnemy)
	{
		if (bDrawingBow)
		{
			AddControllerYawInput(value * 0.2f);
		}
		else AddControllerYawInput(value);
	}
}

void ACPP_DarkLifeCharacter::Sprint()
{
	if ((CurrentCharacterMovement != ECharacterMovement::Ladder) && (CurrentCharacterState == ECharacterState::Normal))
	{
		if (bBlocking) {
	
				RuneActivation(0);
				return;
			
		}


		if (!bBeastPowerMovement)
		{
			if (!bSprintKeyPress)
			{
				if (bCanThrowProjectile)
				{
					ThrowDeactivate();
					SetCharacterMovement(ECharacterMovement::Sprint);
					return;
				}


				if(bIsCrouched && !CheckObstacleAbove(100.0,30.0, EDrawDebugTrace::None))
				{
					SetCharacterMovement(ECharacterMovement::Sprint);
					return;
				}

				if (!bIsCrouched)
				{
					SetCharacterMovement(ECharacterMovement::Sprint);
					return;
				}
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

	if (CharacterParams)
	{
		LockRadius = CharacterParams->LockRadius;
	}

	if (AbilityComponent)
	{
		for (TSubclassOf<UGameplayAbility>& Ability : DefaultAbilities)
		{
			if (Ability)
			{
				AbilityComponent->GiveAbility(Ability);
			}
		}
	}
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
	if ((InventoryManager) && (InventoryManager->bUseCustomInitItems == false))
	{
		InventoryManager->LoadSavedInfo(SaveGame);
	}


	if (CurrentCharacterState == ECharacterState::Injuried_Sword)
	{
		bool retFlag;
		SetFakeExcalibur(retFlag);
		if (retFlag) return;
	}
	else if (CurrentCharacterState == ECharacterState::Normal)
	{
		SetExcalibur();
		SetBow();
		SetShield();
	}
	else if (CurrentCharacterState == ECharacterState::Injuried)
	{
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

	if (SaveGame)
	{
		InventoryManager->MainInventory = SaveGame->Inventory;
	}
}


void ACPP_DarkLifeCharacter::LoadParameters()
{
	UCPP_GameInstance* DLGameInstance = Cast<UCPP_GameInstance>(GetGameInstance());
	UCPP_DarkLifeSaveGame* SaveGame = DLGameInstance->SaveGameObject;
	if (SaveGame)
	{
		SaveGame->ParametersCalculation();
		Health = SaveGame->CurrentHealth;
		MaxHealth = SaveGame->CurrentHealth;
		Stamina = SaveGame->CurrentStamina;
		MaxStamina = SaveGame->CurrentStamina;
		Fracture = SaveGame->CurrentFracture;
		Defense = SaveGame->CurrentDefense;
		Recharge = SaveGame->CurrentRecharge;
		MaxBeast = SaveGame->CurrentBeast;
	}
}

void ACPP_DarkLifeCharacter::SetExcalibur()
{
	// Comprobaci�n de puntero nulo para Excalibur
	if (Excalibur)
	{
		// Si Excalibur tiene un ChildActor, destr�yelo
		if (Excalibur->GetChildActor())
		{
			Excalibur->GetChildActor()->Destroy();
		}

		// Cargar la clase del Blueprint de Excalibur
		UClass* ExcaliburClass = StaticLoadClass(AActor::StaticClass(), nullptr,
		                                         TEXT(
			                                         "/Game/TESTING/Character/Excalibur/Modular_Fantasy_Sword/Blueprints/BP_DarkLifeModularSword.BP_DarkLifeModularSword_C"));
		if (ExcaliburClass)
		{
			Excalibur->SetChildActorClass(ExcaliburClass);
			LoadParameters();
		}
	}
}

void ACPP_DarkLifeCharacter::SetBow()
{
	// Comprobaci�n de puntero nulo para LongBow
	if (LongBow)
	{
		// Si LongBow tiene un ChildActor, destr�yelo
		if (LongBow->GetChildActor())
		{
			LongBow->GetChildActor()->Destroy();
		}

		// Cargar la clase del Blueprint del Arco
		UClass* BowClass = StaticLoadClass(AActor::StaticClass(), nullptr,
		                                   TEXT("/Game/TESTING/Character/Bow/BP_Bow.BP_Bow_C"));
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
			// Comprobar si ShieldSoftReference es v�lido
			if (ShieldSoftReference.IsValid())
			{
				ShieldMesh->SetStaticMesh(ShieldSoftReference.Get());
			}
			else
			{
				// Cargar sincr�nicamente el recurso si no es v�lido
				ShieldSoftReference.LoadSynchronous();
				if (ShieldSoftReference.IsValid())
				{
					ShieldMesh->SetStaticMesh(ShieldSoftReference.Get());
				}
			}
		}
	}
}



void ACPP_DarkLifeCharacter::StaminaIncrease()
{
	if (bBlocking)
	{
		Stamina = UKismetMathLibrary::FClamp(Stamina + 0.15, 0.0, MaxStamina);
	}
	else
	{
		Stamina = UKismetMathLibrary::FClamp(Stamina + 2.0, 0.0, MaxStamina);
	}


	if (Stamina == MaxStamina)
	{
		GetWorldTimerManager().PauseTimer(StaminaIncreaseHandle);
	}
}

void ACPP_DarkLifeCharacter::StaminaDecrease(double decreaseratio)
{
	if (!bStaminaBoost)
	{
		Stamina = UKismetMathLibrary::FClamp(Stamina + (-(decreaseratio)), 0.0f, MaxStamina);
		if (Stamina <= 0.0f)
		{
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
	GetWorldTimerManager().SetTimer(StaminaIncreaseHandle, this, &ACPP_DarkLifeCharacter::StaminaIncrease,
	                                StaminaIncreaseTime, true, StaminaIncreaseDelay);

	//Reset Combo Counter after 1.5 sec no loop
	GetWorldTimerManager().SetTimer(ResetComboCounterHandle, this, &ACPP_DarkLifeCharacter::ResetComboCounter, 1.5f,
	                                false, 1.5f);
}

void ACPP_DarkLifeCharacter::StopSprint()
{
	if (!bSlowRun)
	{
		bSprintKeyPress = false;
		if (bBeastPowerMovement)
		{
			GetCharacterMovement()->MaxWalkSpeed = BeastPowerSpeed;
		}
		else
		{
			if (bWalk)
			{
				if (bCrouched) { GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed; }
				else { GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; }
			}

			else if (bSlowRun)
			{
				if (bCrouched) { GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed; }
				else { GetCharacterMovement()->MaxWalkSpeed = RunSlowSpeed; }
			}

			else
			{
				if (bCrouched) { GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed; }
				else { GetCharacterMovement()->MaxWalkSpeed = RunSpeed; }
			}
		}
	}

	bSprint = false;

	if (StaminaDecreaseHandle.IsValid())
	{
		GetWorldTimerManager().PauseTimer(StaminaDecreaseHandle);
		GetWorldTimerManager().ClearTimer(StaminaDecreaseHandle);
	}

	if (bLockedEnemy)
	{
		SpringArm->CameraLagSpeed = 30.0f;
	}

	GetWorldTimerManager().SetTimer(StaminaIncreaseHandle, this, &ACPP_DarkLifeCharacter::StaminaIncrease,
	                                StaminaIncreaseTime, true, 1.0f);
}

void ACPP_DarkLifeCharacter::StartSprint()
{
	bSprintKeyPress = true;
	if ((GetInputAxisValue("MoveForward") > 0) && (Stamina > 0.0f) && (!bSlowRun))
	{
		GetCharacterMovement()->MaxWalkSpeed = 700.0f;
		bSprint = true;
		SpringArm->bEnableCameraLag = true;
		SpringArm->CameraLagSpeed = 15.0f;

		if (StaminaIncreaseHandle.IsValid())
		{
			GetWorldTimerManager().PauseTimer(StaminaIncreaseHandle);
			GetWorldTimerManager().ClearTimer(StaminaIncreaseHandle);
		}

		double StaminaDecreaseRatio = 0.5;
		if (CharacterParams)
		{
			StaminaDecreaseRatio = CharacterParams->SprintStaminaDecrease;
		}
		
		FTimerDelegate StaminaDecreaseTimerDel;
        StaminaDecreaseTimerDel.BindUFunction(this, FName("StaminaDecrease"),StaminaDecreaseRatio,FString("StaminaDecrease"));
		GetWorldTimerManager().SetTimer(StaminaDecreaseHandle, StaminaDecreaseTimerDel,
		                                StaminaIncreaseTime, true, 0.0f);
	}
	else
	{
		bSprintKeyPress = false;
	}
}

void ACPP_DarkLifeCharacter::SetCharacterMovement(ECharacterMovement NewMovement)
{
	CurrentCharacterMovement = NewMovement;
	CharacterMovementChange.Broadcast(NewMovement);
	JumpActivation(true);
	switch (CurrentCharacterMovement)
	{
	case ECharacterMovement::Walk:
		StopSprint();
		SetWalkSpeed();
		bWalk = true;
		bCrouched = false;
		if (GetCharacterMovement()->IsCrouching())
		{
			UnCrouch();
			
		}
		break;
	case ECharacterMovement::Jog:
		StopSprint();
		SetRunSpeed();
		bWalk = false;
		bCrouched = false;
		if (GetCharacterMovement()->IsCrouching())
		{
			UnCrouch();
		}
		break;
	case ECharacterMovement::Sprint:
		StopAnimMontage(GetCurrentMontage());
		ResetCombo();
		if (GetCharacterMovement()->IsCrouching())
		{
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
		if (GetCharacterMovement()->IsCrouching())
		{
			UnCrouch();
		}
		break;
	case ECharacterMovement::Injuried:
		SetWalkSpeed();
		bWalk = true;
		StopSprint();
		if (GetCharacterMovement()->IsCrouching())
		{
			UnCrouch();
		}
		break;
	case ECharacterMovement::Injuried_Sword:
		SetWalkSpeed();
		bWalk = true;
		StopSprint();
		if (GetCharacterMovement()->IsCrouching())
		{
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
	PlayAnimMontage(DrawBowAnimation, 1.0f, "Default");
}

void ACPP_DarkLifeCharacter::SetTorchActive(bool bActivate, bool bRestorePreviousState)
{
	if (!bRestorePreviousState)
	{
		bTorchActive = bActivate;
		bTorchPreviousState = bTorchActive;
		Torch->SetHiddenInGame(!bTorchActive, true);
	}
	else
	{
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
	Health = UKismetMathLibrary::FClamp(Health + value, 0.0f, MaxHealth);
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
	if ((bJump) && (CurrentCharacterState == ECharacterState::Normal))
	{
		bJump = false;
		StopSprint();
		Jump();
	}
}

void ACPP_DarkLifeCharacter::HandWeaponsVisibility(bool hide)
{
	if (bTorchActive)
	{
		Torch->SetHiddenInGame(hide, true);
	}

	if (bDrawSword)
	{
		Excalibur->SetHiddenInGame(hide, true);
	}


	if (bDrawShield)
	{
		ShieldMesh->SetHiddenInGame(hide, true);
	}
}

void ACPP_DarkLifeCharacter::PlayAnimationByCharacterState(int32 animationIndex, bool& Success)
{
	Success = false;

	if (bTorchUp)
	{
		if (GetCurrentMontage() != ShieldAttackAnimations[2] && ShieldAttackAnimations.IsValidIndex(0))
		{
			PlayAnimMontage(ShieldAttackAnimations[2]);
			//Stamina = 0;
			Success = true;
			return;
		}
	}

	if (!bBlocking)
	{
		if (bSprint)
		{
			if (AttackOnSprintAnimations.IsValidIndex((int32)CombatState))
			{
				PlayAnimMontage(AttackOnSprintAnimations[(int32)CombatState]);
				UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(GetWorld(), ChargeAttackTimer);
				Success = true;
			}
		}
		else
		{
			if ((!CurrentStateAnimations.IsEmpty()) && (CurrentStateAnimations.IsValidIndex(ComboCounter)))
			{
				if ((animationIndex + 1) == CurrentStateAnimations.Num())
				{
					ComboCounter = 0;
					PlayAnimMontage(CurrentStateAnimations[animationIndex]);
					Attacking.Broadcast();
					Success = true;
				}
				else
				{
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


	else
	{
		switch (CombatState)
		{
			case ECharacterCombatState::OneHandShield:
				if (GetCurrentMontage() != ShieldAttackAnimations[0] && ShieldAttackAnimations.IsValidIndex(0))
				{
					PlayAnimMontage(ShieldAttackAnimations[0]);
					//Stamina = 0;
					Success = true;
				}
				break;
		case ECharacterCombatState::TwoHandSword:
			if (GetCurrentMontage() != ShieldAttackAnimations[1] && ShieldAttackAnimations.IsValidIndex(1))
			{
				PlayAnimMontage(ShieldAttackAnimations[1]);
				//Stamina = 0;
				Success = true;
			}
			break;
		case ECharacterCombatState::OneHandSword:
			if (GetCurrentMontage() != ShieldAttackAnimations[0] && ShieldAttackAnimations.IsValidIndex(0))
			{
				PlayAnimMontage(ShieldAttackAnimations[0]);
				//Stamina = 0;
				Success = true;
			}
			break;
		case ECharacterCombatState::OneHandTorch:
			break;
		case ECharacterCombatState::TwoBareHand:
			break;
		default:
			break;
		}
		
		
	}


	UpdateStaminaByCharacterCombatState();
	//StopSprint();
}

void ACPP_DarkLifeCharacter::SaveComboAttack()
{
	if (bSaveAttack)
	{
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
		if (!bDrawShield)
		{
			SetTorchActive(true, false);
		}
		else
		{
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

void ACPP_DarkLifeCharacter::HitAnimation(FHitResult HitInfo, EEnemyDamageType DamageType, AActor* CauserReference)
{
	FVector ImpactNormal;
	bool bHitDuringStuntAnimation = false;
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f,FColor::Green, UEnum::GetValueAsString(CurrentCharacterNegativeStatus));
	if (CauserReference)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, CauserReference->GetClass()->GetName());
		ImpactNormal = (CauserReference->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	}
	else
	{
		ImpactNormal = HitInfo.ImpactNormal;
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "NoCauserReference");
	}

	switch (DamageType)
	{
	case EEnemyDamageType::RegularDamage:

		if (bBlocking)
		{
			PlayBlockingHitAnimations();
		}
		else
		{
			if (GetCurrentMontage() == StuntHitAnimations[0] || GetCurrentMontage() == StuntAnimations[0])
			{
				break;
			}
			PlayRegularDamageHitAnimation(ImpactNormal);
			bBlocking = false;
		}

		break;
	case EEnemyDamageType::StrongDamage:
		break;
	case EEnemyDamageType::StuntDamage:

		PlayStuntHitAnimations(bHitDuringStuntAnimation);
		bBlocking = false;
		if (bHitDuringStuntAnimation == true)
		{
			break;
		}
		PlayStuntDamageHitAnimation();
		break;
	default:
		break;
	}
}

void ACPP_DarkLifeCharacter::PlayRegularDamageHitAnimation(FVector ImpactNormal)
{
	if (HitAnimations.Num() > 3)
	{
		// Back Hit Animation 
		if (HitAnimations.IsValidIndex(3) && HitAngleInRange(ImpactNormal, GetActorForwardVector(), 100.0f, 180.0f,
		                                                     true, true) && (GetCurrentMontage() != HitAnimations[3]))
		{
			PlayAnimMontage(HitAnimations[3]);
		}
		// Right Hit Animation 
		else if (HitAnimations.IsValidIndex(2) && HitAngleInRange(ImpactNormal, GetActorRightVector(), 0.0f, 90.0f,
		                                                          true, false) && (GetCurrentMontage() != HitAnimations[
			2]))
		{
			PlayAnimMontage(HitAnimations[2]);
		}
		// Left Hit Animation 
		else if (HitAnimations.IsValidIndex(1) && HitAngleInRange(ImpactNormal, -GetActorRightVector(), 0.0f, 90.0f,
		                                                          true, false) && (GetCurrentMontage() != HitAnimations[
			1]))
		{
			PlayAnimMontage(HitAnimations[1]);
		}
		// Front Hit Animation 
		else if (HitAnimations.IsValidIndex(0) && HitAngleInRange(ImpactNormal, -GetActorForwardVector(), 0.0f, 90.0f,
		                                                          true, true) && (GetCurrentMontage() != HitAnimations[
			0]))
		{
			PlayAnimMontage(HitAnimations[0]);
		}
	}
	else if (HitAnimations.IsValidIndex(0) && (GetCurrentMontage() != HitAnimations[0]))
	{
		PlayAnimMontage(HitAnimations[0]);
	}
}


void ACPP_DarkLifeCharacter::PlayStuntDamageHitAnimation()
{
	if ((CombatState == ECharacterCombatState::OneHandSword) || (CombatState == ECharacterCombatState::OneHandShield))
	{
		if (StuntAnimations.IsValidIndex(0) && IsValid(StuntAnimations[0]))
		{
			if (GetCurrentMontage() != StuntAnimations[0])
			{
				PlayAnimMontage(StuntAnimations[0]);
			}
		}
	}
}

void ACPP_DarkLifeCharacter::SetCharacterNegativeStatus(ECharacterNegativeStatus NewNegativeStatus)
{
	CurrentCharacterNegativeStatus = NewNegativeStatus;
	//GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Blue, UEnum::GetValueAsString(CurrentCharacterNegativeStatus));	
}

void ACPP_DarkLifeCharacter::PlayParryFinisherAnimation(int parryIndex, UAnimMontage*& ParryMontage,
                                                        TArray<UAnimMontage*>& ParryAnimList)
{
	if (ParryAnimList.IsValidIndex(parryIndex))
	{
		if (IsValid(ParryAnimList[parryIndex]))
		{
			PlayAnimMontage(ParryAnimList[parryIndex]);
			ParryMontage = ParryAnimList[parryIndex];
		}
	}
}

void ACPP_DarkLifeCharacter::PlayFromTheBackFinisherAnimation(int backFinishIndex, UAnimMontage*& FromTheBackMontage)
{
	if (BackFinisher.IsValidIndex(backFinishIndex))
	{
		if (IsValid(BackFinisher[backFinishIndex]))
		{
			PlayAnimMontage(BackFinisher[backFinishIndex]);
			FromTheBackMontage = BackFinisher[backFinishIndex];
		}
	}
}

void ACPP_DarkLifeCharacter::PlayRandomFinishAnimation(ECharacterFinishMoveType FinishMovementType,
                                                       double& AnimationLength, bool& Success)
{
	Success = true;
	if (IsValid(CurrentEnemy))
	{
		ACPP_Enemy* EnemyRef = Cast<ACPP_Enemy>(CurrentEnemy);
		if (IsValid(EnemyRef))
		{
			if ((EnemyRef->bCanBeExecuted) && (EnemyRef->bExecutionActive))
			{
				int randomIndex = 0;
				UAnimMontage* ParryMontage = nullptr;
				UAnimMontage* BackMontage = nullptr;
				TArray<UAnimMontage*> LocalParryFinisherAnimation;

				switch (FinishMovementType)
				{
				case ECharacterFinishMoveType::AfterParry:

					if (CombatState == ECharacterCombatState::TwoHandSword)
					{
						LocalParryFinisherAnimation = TwoHandsParryFinisher;
					}
					else LocalParryFinisherAnimation = OneHandParryFinisher;

					if (!LocalParryFinisherAnimation.IsEmpty())
					{
						randomIndex = UKismetMathLibrary::RandomInteger(LocalParryFinisherAnimation.Num());
						UAIBlueprintHelperLibrary::GetAIController(EnemyRef)->GetBrainComponent()->PauseLogic(
							"Finisher Received");
						EnemyRef->GetMesh()->Stop();
						PlayParryFinisherAnimation(randomIndex, ParryMontage, LocalParryFinisherAnimation);
						EnemyRef->PlayParryFinisherAnimation(randomIndex, CombatState);
						AnimationLength = ParryMontage->GetPlayLength();
						Success = true;
						ResetCombo();
						break;
					}

					else
					{
						Success = false;
						break;
					}
				case ECharacterFinishMoveType::FromTheBack:

					if (!BackFinisher.IsEmpty())
					{
						randomIndex = UKismetMathLibrary::RandomInteger(BackFinisher.Num());
						EnemyRef->GetMesh()->Stop();
						UAIBlueprintHelperLibrary::GetAIController(EnemyRef)->GetBrainComponent()->PauseLogic(
							"Finisher Received");
						PlayFromTheBackFinisherAnimation(randomIndex, BackMontage);
						EnemyRef->PlayFromTheBackFinisherAnimation(randomIndex);
						AnimationLength = BackMontage->GetPlayLength();
						Success = true;
						ResetCombo();
						break;
					}
					else
					{
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

	if (ChargeAttackArray.Num() > 0)
	{
		AnimationsCount = UKismetMathLibrary::RandomInteger(ChargeAttackArray.Num());
		if (ChargeAttackArray.IsValidIndex(AnimationsCount))
		{
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
		if (CombatState == ECharacterCombatState::OneHandSword)
		{
			if (OneHandDeflectedAnimations.IsValidIndex(CustomComboIndex) && (IsValid(
				OneHandDeflectedAnimations[CustomComboIndex])))
			{
				PlayAnimMontage(OneHandDeflectedAnimations[CustomComboIndex]);
			}
		}
		else if (CombatState == ECharacterCombatState::TwoHandSword)
		{
			if (TwoHandsDeflectedAnimations.IsValidIndex(CustomComboIndex) && (IsValid(
				TwoHandsDeflectedAnimations[CustomComboIndex])))
			{
				PlayAnimMontage(TwoHandsDeflectedAnimations[CustomComboIndex]);
			}
		}
		break;
	case ECharacterDamageType::Shield:
		if (ShieldDeflectedAnimations.IsValidIndex(CustomComboIndex) && (IsValid(
			ShieldDeflectedAnimations[CustomComboIndex])))
		{
			PlayAnimMontage(ShieldDeflectedAnimations[CustomComboIndex]);
		}
		break;
	case ECharacterDamageType::Torch:
		break;
	case ECharacterDamageType::Punch:
		if (PunchDeflectedAnimations.IsValidIndex(CustomComboIndex) && (IsValid(
			PunchDeflectedAnimations[CustomComboIndex])))
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
	if (BlockAnimations.Find(CombatState) && (IsValid(BlockAnimations[CombatState])))
	{
		if (GetCurrentMontage() != BlockAnimations[CombatState])
		{
			StopAnimMontage(GetCurrentMontage());
			PlayAnimMontage(BlockAnimations[CombatState]);
		}
	}
}

void ACPP_DarkLifeCharacter::PlayBlockingHitAnimations()
{
	if (BlockHitAnimations.Find(CombatState) && (IsValid(BlockHitAnimations[CombatState])))
	{
		if (GetCurrentMontage() != BlockHitAnimations[CombatState])
		{
			PlayAnimMontage(BlockHitAnimations[CombatState]);
		}
	}
}

void ACPP_DarkLifeCharacter::SetCharacterState(ECharacterState NewCharacterstate)
{
	CurrentCharacterState = NewCharacterstate;

	switch (CurrentCharacterState)
	{
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
	if (DLGameInstance)
	{
		DLGameInstance->SaveGame();
	}
}

void ACPP_DarkLifeCharacter::SetFakeExcalibur(bool& retFlag)
{
	retFlag = true;
	static const FString FakeExcaliburPath = TEXT(
		"/Game/TESTING/Character/Excalibur/Modular_Fantasy_Sword/Blueprints/BP_FakeExcalibur.BP_FakeExcalibur_C");
	UClass* FakeExcaliburClass = StaticLoadClass(AActor::StaticClass(), nullptr, *FakeExcaliburPath);

	if (!FakeExcaliburClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	FakeExcaliburActorRef = GetWorld()->SpawnActor<AActor>(FakeExcaliburClass, FVector::ZeroVector,
	                                                       FRotator::ZeroRotator, SpawnParameters);

	if (FakeExcaliburActorRef)
	{
		USkeletalMeshComponent* FakeExcaliburMesh = GetMesh();
		if (FakeExcaliburMesh)
		{
			if (FakeExcaliburMesh->DoesSocketExist(TEXT("Sword")))
			{
				FakeExcaliburActorRef->AttachToComponent(FakeExcaliburMesh,
				                                         FAttachmentTransformRules::KeepRelativeTransform,
				                                         TEXT("Sword"));

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

void ACPP_DarkLifeCharacter::CharacterDrawSword(bool bOnlyToBack, bool bForceEquipment, bool bForceUnequipment)
{
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative,
	                                          EAttachmentRule::KeepRelative, true);
	
	if (bForceEquipment && !bForceUnequipment) {
		EquipExcalibur();
		ResetCombo();
		return;
	}
	else if (!bForceEquipment && bForceUnequipment)
	{
		UnequipExcalibur();
		ResetCombo();
		return;
	}

	if ((bDrawSword) || (bOnlyToBack))
	{
		UnequipExcalibur();
	}

	else
	{
		EquipExcalibur();


		ResetCombo();
		//PlayerCharacter->StopSprint();
	}
}

void ACPP_DarkLifeCharacter::UnequipExcalibur()
{

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative,
		EAttachmentRule::KeepRelative, true);


	Excalibur->AttachToComponent(GetMesh(), AttachmentRules, "Sword_Back");

	if (bDrawShield)
	{
		SetCombatState(ECharacterCombatState::OneHandShield);
	}
	else if (bTorchActive)
	{
		SetCombatState(ECharacterCombatState::OneHandTorch);
	}
	else
	{
		SetCombatState(ECharacterCombatState::TwoBareHand);
	}
}

void ACPP_DarkLifeCharacter::EquipExcalibur()
{

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative,
		EAttachmentRule::KeepRelative, true);

	Excalibur->AttachToComponent(GetMesh(), AttachmentRules, "Sword");
	if ((bDrawShield) || (bTorchActive))
	{
		SetCombatState(ECharacterCombatState::OneHandSword);
	}
	else
	{
		SetCombatState(ECharacterCombatState::TwoHandSword);
	}
}

void ACPP_DarkLifeCharacter::CharacterDrawShield(bool bOnlyToBack, bool bForceEquipment, bool bForceUnequipment)
{

	if (bForceEquipment && !bForceUnequipment) {
		EquipShield();
		ResetCombo();
		return;
	}
	else if (!bForceEquipment && bForceUnequipment)
	{
		UnequipShield();
		ResetCombo();
		return;
	}

	if ((bDrawShield) || (bOnlyToBack))
	{
		UnequipShield();
	}
	else
	{
		EquipShield();
	}

	ResetCombo();
	//PlayerCharacter->StopSprint();
}

void ACPP_DarkLifeCharacter::UnequipShield()
{
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative,
		EAttachmentRule::KeepRelative, true);

	ShieldMesh->AttachToComponent(GetMesh(), AttachmentRules, "Shield_Back");
	if (bDrawSword)
	{
		SetCombatState(ECharacterCombatState::TwoHandSword);
	}

	else
	{
		SetCombatState(ECharacterCombatState::TwoBareHand);
	}
}

void ACPP_DarkLifeCharacter::EquipShield()
{

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative,
		EAttachmentRule::KeepRelative, true);

	ShieldMesh->AttachToComponent(GetMesh(), AttachmentRules, "Shield");
	if (bDrawSword)
	{
		bDrawShield = true;
		SetCombatState(ECharacterCombatState::OneHandSword);
	}
	else
	{
		SetCombatState(ECharacterCombatState::OneHandShield);
	}
}

void ACPP_DarkLifeCharacter::PlayStuntHitAnimations(bool& bSuccess)
{
	bSuccess = false;

	if (StuntHitAnimations.IsValidIndex(0) && IsValid(StuntHitAnimations[0]))
	{
		if (GetCurrentMontage() == StuntAnimations[0] && GetMesh()->GetAnimInstance()->
		                                                            Montage_GetCurrentSection(StuntAnimations[0]) ==
			"Stunt")
		{
			PlayAnimMontage(StuntHitAnimations[0]);
			bSuccess = true;
		}
		else if (GetCurrentMontage() == StuntHitAnimations[0])
		{
			bSuccess = true;
		}
	}
}

void ACPP_DarkLifeCharacter::WeaponsByCharacterState(ECharacterCombatState NewCombatState)
{
	switch (NewCombatState)
	{
	case ECharacterCombatState::OneHandSword:
		CharacterDrawSword(false,true,false);
		CharacterDrawShield(false,true, false);
		break;
	case ECharacterCombatState::TwoHandSword:
		CharacterDrawSword(false,true,false);
		CharacterDrawShield(true,false,true);
		break;
	case ECharacterCombatState::OneHandShield:
		CharacterDrawSword(true,false,true);
		CharacterDrawShield(false,true,false);
		break;
	case ECharacterCombatState::OneHandTorch:
		CharacterDrawSword(true,false,true);
		CharacterDrawShield(true,false,true);
		break;
	case ECharacterCombatState::TwoBareHand:
		CharacterDrawSword(true,false,true);
		CharacterDrawShield(true,false,true);
		break;
	default:
		break;
	}
}

bool ACPP_DarkLifeCharacter::CheckObstacleAbove(double TraceDistance, double TraceRadius, EDrawDebugTrace::Type DrawDebugTrace )
{
	
	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = (GetActorUpVector() * TraceDistance) + TraceStart;
	FHitResult HitResult;
	const TArray<AActor*> ActorsToIgnore = {this};
	
	
	
	bool bObjectDetected = UKismetSystemLibrary::SphereTraceSingle(GetWorld(),TraceStart,TraceEnd,TraceRadius,ETraceTypeQuery::TraceTypeQuery1,false,ActorsToIgnore,DrawDebugTrace, HitResult,true);
	return bObjectDetected;
}

void ACPP_DarkLifeCharacter::CancelChargeAttack()
{
	UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(GetWorld(), ChargeAttackTimer);
}

void ACPP_DarkLifeCharacter::RuneActivation(int RuneSlot)
{
	TObjectPtr<UCPP_GameInstance> DLGameInstance = Cast<UCPP_GameInstance>(GetGameInstance());
	TObjectPtr<UCPP_DA_Item_Rune> Rune = InventoryManager->GetRuneBySlotIndex(RuneSlot);
	
	if (Rune && DLGameInstance) {

		bool bIsRuneOnCooldown = DLGameInstance->IsRuneOnCooldown(RuneSlot);

		if (bIsRuneOnCooldown) return;

		DLGameInstance->RunesUseCoolDown(InventoryManager->GetRuneBySlotIndex(RuneSlot), RuneSlot);
		Rune->UseItem(this);
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
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACPP_DarkLifeCharacter::PerformJump);
	PlayerInputComponent->BindAxis("LookUp", this, &ACPP_DarkLifeCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &ACPP_DarkLifeCharacter::Turn);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ACPP_DarkLifeCharacter::Sprint);
}
