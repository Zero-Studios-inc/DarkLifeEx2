// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CPP_CharacterDrawSword.h"

void UCPP_CharacterDrawSword::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration)
{
	ACPP_DarkLifeCharacter* PlayerCharacter = Cast<ACPP_DarkLifeCharacter>(MeshComp->GetOwner());
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true);
	if (IsValid(PlayerCharacter)) {

		if (PlayerCharacter->bDrawSword) {

			PlayerCharacter->Excalibur->AttachToComponent(PlayerCharacter->GetMesh(), AttachmentRules, "Sword_Back");

			if (PlayerCharacter->bDrawShield) {
				PlayerCharacter->SetCombatState(ECharacterCombatState::OneHandShield);
			}
			else if (PlayerCharacter->bTorchActive) {
				PlayerCharacter->SetCombatState(ECharacterCombatState::OneHandTorch);
			}
			else {
				PlayerCharacter->SetCombatState(ECharacterCombatState::TwoBareHand);
			}
		}

		else {
			PlayerCharacter->Excalibur->AttachToComponent(PlayerCharacter->GetMesh(), AttachmentRules, "Sword");
			if ((PlayerCharacter->bDrawShield) || (PlayerCharacter->bTorchActive)) {
				
				PlayerCharacter->SetCombatState(ECharacterCombatState::OneHandSword);
			}
			else {
				PlayerCharacter->SetCombatState(ECharacterCombatState::TwoHandSword);
			}

			
			PlayerCharacter->ResetCombo();
			//PlayerCharacter->StopSprint();
		}
	}
}
