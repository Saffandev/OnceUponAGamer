// Copyright Epic Games, Inc. All Rights Reserved.

#include "OnceUponAGamerGameMode.h"
#include "OnceUponAGamerHUD.h"
#include "OnceUponAGamerCharacter.h"
#include "UObject/ConstructorHelpers.h"

AOnceUponAGamerGameMode::AOnceUponAGamerGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	// DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AOnceUponAGamerHUD::StaticClass();
}
