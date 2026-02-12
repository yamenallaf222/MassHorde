// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityElementTypes.h"
#include "MassHordeFragments.generated.h"

/**
 *
 */

USTRUCT() struct MASSHORDE_API FMassHordeWildlingTag : public FMassTag { GENERATED_BODY() };
USTRUCT() struct MASSHORDE_API FPlayerVisible : public FMassTag { GENERATED_BODY() };
USTRUCT() struct MASSHORDE_API FEntityDeadAndPooledTag : public FMassTag { GENERATED_BODY() };
USTRUCT() struct MASSHORDE_API FMovingTag : public FMassTag { GENERATED_BODY() };


USTRUCT()
struct FHealthFragment : public FMassFragment
{
	GENERATED_BODY()
	
	float CurrentHealth;
};



USTRUCT()
struct FMassHordeSharedFragment : public FMassSharedFragment
{
	GENERATED_BODY()
	
	FVector PlayerLocation;
	
};
