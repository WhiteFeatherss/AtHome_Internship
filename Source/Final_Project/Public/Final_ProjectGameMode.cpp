// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Final_ProjectGameMode.h"
#include "MC_Behaviour.h"
#include "UObject/ConstructorHelpers.h"

AFinal_ProjectGameMode::AFinal_ProjectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/MainCharacter/BP_MyMC_Behaviour"));
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonLongsword/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;


}
