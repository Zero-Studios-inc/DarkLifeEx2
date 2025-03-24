// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Items/Runes/CPP_DA_Item_Rune_Attack.h"
#include "Core/CPP_DarkLifeCharacter.h"

void UCPP_DA_Item_Rune_Attack::UseItem(ACPP_DarkLifeCharacter* CharacterRef)
{
	if(CharacterRef)
	{
		if (IsValid(UseItemActionAnimation)) {
			CharacterRef->PlayAnimMontage(UseItemActionAnimation);
		}
	}
}
