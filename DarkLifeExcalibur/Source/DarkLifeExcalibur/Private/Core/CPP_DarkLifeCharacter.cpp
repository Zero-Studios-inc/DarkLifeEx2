// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CPP_DarkLifeCharacter.h"

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
	WalkSpeed = 150.0;
	RunSpeed = 350.0;
	BeastPowerSpeed = 5000.0;
	bSprint = false;
	bLockedEnemy = false;
	bDrawingShield = false;
	bDrawShield = false;
	bDrawSword = false;
	bDrawingSword = false;
	bTorchActive = false;
	
	
}

// Called when the game starts or when spawned
void ACPP_DarkLifeCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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
					SpringArm->bEnableCameraLag = true;
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
	Stamina = UKismetMathLibrary::FClamp(Stamina + (-0.3f),0.0f,MaxStamina);
	if (Stamina <= 0.0f) {
		StopSprint();
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
	bSprintKeyPress = false;
	if (bBeastPowerMovement) {
		GetCharacterMovement()->MaxWalkSpeed = BeastPowerSpeed;
	}
	else {
		if (bWalk) {
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		}
		else {
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		}
	}

	bSprint = false;

	if (StaminaIncreaseHandle.IsValid()) {
		GetWorldTimerManager().PauseTimer(StaminaDecreaseHandle);
		GetWorldTimerManager().ClearTimer(StaminaDecreaseHandle);
	}

	if (bLockedEnemy) {
		SpringArm->CameraLagSpeed = 30.0f;
	}

	GetWorldTimerManager().SetTimer(StaminaIncreaseHandle, this, &ACPP_DarkLifeCharacter::StaminaIncrease, StaminaIncreaseTime, true, 0.0f);

}

void ACPP_DarkLifeCharacter::ShootArrow()
{
	bDrawFinish = false;
	//PlayAnimMontage(DrawBowAnimation);
	PlayAnimMontage(ReadyBowAnimation,1.0f,"Aim");
}

void ACPP_DarkLifeCharacter::SetTorchActive(bool bActivate)
{
	bTorchActive = bActivate;
	Torch->SetHiddenInGame(!bTorchActive,true);
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

}

