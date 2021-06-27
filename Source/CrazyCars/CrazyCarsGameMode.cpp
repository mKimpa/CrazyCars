// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrazyCarsGameMode.h"
#include "CrazyCarsPawn.h"
#include "CrazyCarsHud.h"

ACrazyCarsGameMode::ACrazyCarsGameMode()
{
	DefaultPawnClass = ACrazyCarsPawn::StaticClass();
	HUDClass = ACrazyCarsHud::StaticClass();
}
