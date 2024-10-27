// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Actors/CPP_ProjectileSpell.h"

// Sets default values
ACPP_ProjectileSpell::ACPP_ProjectileSpell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Init Components
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	RootComponent = SphereCollision;
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	
	Spell = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpellVFX"));
	check(Spell);
	Spell->SetupAttachment(RootComponent);
	SpellHit = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpellHitVFX"));
	check(SpellHit);
	SpellHit->SetupAttachment(RootComponent);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	
}

// Called when the game starts or when spawned
void ACPP_ProjectileSpell::BeginPlay()
{
	Super::BeginPlay();
	
	if (ProjectileSpellSettings)
	{
		SpellImpulse = ProjectileSpellSettings->SpellImpulse;
		SpellDamage = ProjectileSpellSettings->SpellDamage;
		SpellStaminaDamage = ProjectileSpellSettings->SpellStaminaDamage;
		SpellRotation = ProjectileSpellSettings->SpellRotation;
		SpellScale = ProjectileSpellSettings->SpellScale;
		SpellHitRotation = ProjectileSpellSettings->SpellHitRotation;
		SpellHitScale = ProjectileSpellSettings->SpellHitScale;

		if (ProjectileSpellSettings->Spell && Spell) {
			Spell->SetAsset(ProjectileSpellSettings->Spell);
			Spell->SetActive(true, true);
			Spell->SetVisibility(true, true);
		}
		if (ProjectileSpellSettings->SpellHit && SpellHit) {
			SpellHit->SetAsset(ProjectileSpellSettings->SpellHit);
		}
		SphereCollision->SetSphereRadius(ProjectileSpellSettings->SphereCollisionRadius);

		if(ProjectileSpellSettings->SpellHitSound)
		{
			SpellSound = ProjectileSpellSettings->SpellHitSound;
		}

	}

	if (ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) {
		
		FVector PlayerVelocity = PlayerCharacter->GetVelocity();
		double VelocityMultiplier = (GetDistanceTo(PlayerCharacter) / SpellImpulse);
		FVector EndLocation = PlayerCharacter->GetActorLocation() + (VelocityMultiplier * PlayerVelocity);

		FVector ProjectileVelocity = FVector(0,0,0);
		UGameplayStatics::SuggestProjectileVelocity(GetWorld(), ProjectileVelocity, GetActorLocation(), EndLocation, SpellImpulse,false,0.0f,0.0f,ESuggestProjVelocityTraceOption::DoNotTrace);
		ProjectileMovement->Velocity = ProjectileVelocity;
		ProjectileMovement->bRotationFollowsVelocity = true;
		
	}
	
		
}



// Called every frame
void ACPP_ProjectileSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACPP_ProjectileSpell::OnProjectileSpellHit()
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),
		SpellHit->GetAsset(),
		SphereCollision->GetComponentLocation(),
		SpellHitRotation,
		SpellHitScale,
		true,
		true,
		ENCPoolMethod::None,
		true);
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(),SpellSound,SphereCollision->GetComponentLocation());
}




