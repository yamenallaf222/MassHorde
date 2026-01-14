// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mass/Fragments/MassHordeFragments.h"
#include "MassEntityManager.h"



namespace MassHordeUtility
{
	bool HidePassenger(const FMassEntityManager& EntityManager, const FMassEntityHandle& EntityHandle);
	bool ShowPassenger(const FMassEntityManager& EntityManager, const FMassEntityHandle& EntityHandle, const FVector& ShowLocation);
}

