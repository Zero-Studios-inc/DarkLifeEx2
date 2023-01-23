// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_CharacterDrawShield.h"

void UCPP_CharacterDrawShield::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration)
{
	ACPP_DarkLifeCharacter* PlayerCharacter = Cast<ACPP_DarkLifeCharacter>(MeshComp->GetOwner());
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative,true);
	if (IsValid(PlayerCharacter)) {
		if (PlayerCharacter->bDrawShield) {
			PlayerCharacter->ShieldMesh->AttachToComponent(PlayerCharacter->GetMesh(), AttachmentRules, "Shield_Back");
			if (PlayerCharacter->bDrawSword) {
				
					PlayerCharacter->SetCombatState(ECharacterCombatState::TwoHandSword);
				}
				
			else {
				PlayerCharacter->SetCombatState(ECharacterCombatState::TwoBareHand);
			}
		}
		else {
			PlayerCharacter->ShieldMesh->AttachToComponent(PlayerCharacter->GetMesh(), AttachmentRules, "Shield");
			if (PlayerCharacter->bDrawSword) {
				PlayerCharacter->bDrawShield = true;
				PlayerCharacter->SetCombatState(ECharacterCombatState::OneHandSword);
			}
			else {
				PlayerCharacter->SetCombatState(ECharacterCombatState::OneHandShield);
			}
		}

		PlayerCharacter->ResetCombo();
		//PlayerCharacter->StopSprint();
	}
}
