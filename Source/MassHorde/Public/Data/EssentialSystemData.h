

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTemplate.h"
#include "EssentialSystemData.generated.h"

/**
 * 
 */

USTRUCT()
struct MASSHORDE_API FMassHordeSpawnRequest
{
	GENERATED_BODY()

	const FMassEntityTemplate* EntityTemplate = nullptr;
	int32 RemainingCount = 0;
};