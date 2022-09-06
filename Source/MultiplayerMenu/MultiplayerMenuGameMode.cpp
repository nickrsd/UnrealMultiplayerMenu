// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerMenuGameMode.h"
#include "MultiplayerMenuCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMultiplayerMenuGameMode::AMultiplayerMenuGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
