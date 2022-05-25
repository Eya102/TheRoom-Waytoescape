// Copyright Epic Games, Inc. All Rights Reserved.

#include "TheRoomGameMode.h"
#include "TheRoomCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATheRoomGameMode::ATheRoomGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
