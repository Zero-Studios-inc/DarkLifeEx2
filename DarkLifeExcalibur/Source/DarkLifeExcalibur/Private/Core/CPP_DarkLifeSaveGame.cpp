// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CPP_DarkLifeSaveGame.h"

void UCPP_DarkLifeSaveGame::ParametersCalculation()
{
	CalculateHealth();
	CalculateBeast();
	CalculateDefense();
	CalculateFracture();
	CalculateRecharge();
	CalculateStamina();
	
}

void UCPP_DarkLifeSaveGame::CalculateHealth()
{
	CurrentHealth = CharacterHealthBase;
}

void UCPP_DarkLifeSaveGame::CalculateStamina()
{
	CurrentStamina = CharacterStaminaBase;
}

void UCPP_DarkLifeSaveGame::CalculateDefense()
{
	CurrentDefense = CharacterDefenseBase;
}

void UCPP_DarkLifeSaveGame::CalculateBeast()
{
	CurrentBeast = CharacterBeastBase;
}

void UCPP_DarkLifeSaveGame::CalculateFracture()
{
	CurrentFracture = CharacterFractureBase;
	
	if (IsValid(ExcaliburBlade)) {
		CurrentFracture += ExcaliburBlade->Fracture;
	}
	

}

void UCPP_DarkLifeSaveGame::CalculateRecharge()
{
	CurrentRecharge = CharacterRechargeBase;
}
