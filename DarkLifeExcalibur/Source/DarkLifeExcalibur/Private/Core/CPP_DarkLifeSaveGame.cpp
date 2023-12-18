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

FTransform UCPP_DarkLifeSaveGame::GetLevelLastSavePoint(FName CurrentLevel, bool& LevelVisited)
{
	LevelVisited = false;

	if (LastSavePoint.Contains(CurrentLevel)) {
		LevelVisited = true;
		return *LastSavePoint.Find(CurrentLevel);
		//return LastSavePoint.Find(CurrentLevel);
	}

	return FTransform();
}

void UCPP_DarkLifeSaveGame::SetNewSavedLevelTransform(FName LevelName, FTransform NewTransform)
{
	if (LevelLastSavePointLocation.Contains(LevelName)) {
		LevelLastSavePointLocation[LevelName] = NewTransform;
	}
	else {
		LevelLastSavePointLocation.Add(LevelName, NewTransform);
	}
	
}

FTransform UCPP_DarkLifeSaveGame::GetSavedLevelTransform(FName LevelName, bool& ValidLevel)
{
	ValidLevel = false;
	if (LevelLastSavePointLocation.Contains(LevelName)) {
		ValidLevel = true;
		return LevelLastSavePointLocation[LevelName];
	}

	return FTransform();
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
	if (IsValid(Shield)) {
		CurrentDefense += Shield->Defense;
	}
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
