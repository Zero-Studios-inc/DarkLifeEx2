// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Items/CPP_DA_Item_Stamina.h"

void UCPP_DA_Item_Stamina::UseItem(ACPP_DarkLifeCharacter* CharacterRef) {
	if (IsValid(CharacterRef)) {
		CharacterRef->Stamina = UKismetMathLibrary::FClamp(CharacterRef->Stamina + (CharacterRef->MaxStamina / StaminaPercent), 0.0f, CharacterRef->MaxStamina);
		if (IsValid(UseItemActionAnimation)) {
			CharacterRef->PlayAnimMontage(UseItemActionAnimation);
		}

	}
}