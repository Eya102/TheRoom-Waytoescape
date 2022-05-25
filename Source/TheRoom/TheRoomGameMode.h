// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TheRoomGameMode.generated.h"

UCLASS(minimalapi)
class ATheRoomGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATheRoomGameMode();
	class UDataTable* GetItemsDB() const { return itemDB; }


protected:
	UPROPERTY(EditDefaultsOnly)
		class UDataTable* itemDB;


};



