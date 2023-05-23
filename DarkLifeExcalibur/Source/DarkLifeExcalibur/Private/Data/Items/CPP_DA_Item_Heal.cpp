// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Items/CPP_DA_Item_Heal.h"

void UCPP_DA_Item_Heal::UseItem(ACPP_DarkLifeCharacter* CharacterRef) {
	if (IsValid(CharacterRef)) {
		CharacterRef->Health = UKismetMathLibrary::FClamp(CharacterRef->Health + (CharacterRef->MaxHealth / HealPercent), 0.0f, CharacterRef->MaxHealth);
		if (IsValid(UseItemActionAnimation)) {
			CharacterRef->PlayAnimMontage(UseItemActionAnimation);
		}
	}
}