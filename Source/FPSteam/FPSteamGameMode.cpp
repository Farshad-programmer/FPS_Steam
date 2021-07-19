// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSteamGameMode.h"
#include "FPSteamHUD.h"
#include "FPSteamCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFPSteamGameMode::AFPSteamGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSteamHUD::StaticClass();
}
