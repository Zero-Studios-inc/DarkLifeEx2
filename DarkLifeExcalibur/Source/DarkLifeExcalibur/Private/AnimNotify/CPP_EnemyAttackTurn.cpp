// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_EnemyAttackTurn.h"


void UCPP_EnemyAttackTurn::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	AActor* EnemyRef = MeshComp->GetOwner();
	AActor* CharacterRef = UGameplayStatics::GetPlayerPawn(MeshComp, 0);
	if ((EnemyRef) && (CharacterRef)) {
		FVector TargetLocation = CharacterRef->GetActorLocation();
		FVector CurrentLocation = EnemyRef->GetActorLocation();
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, TargetLocation);
		EnemyRef->SetActorRotation(FRotator(EnemyRef->GetActorRotation().Pitch, TargetRotation.Yaw,EnemyRef->GetActorRotation().Roll));
	
	}
	
}




