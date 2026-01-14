// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/MassHordeUtility.h"

#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassRepresentationFragments.h"


bool MassHordeUtility::HidePassenger(const FMassEntityManager& EntityManager, const FMassEntityHandle& EntityHandle)
{
	FTransformFragment* TransformFragment = EntityManager.GetFragmentDataPtr<FTransformFragment>(EntityHandle);
	if (!TransformFragment) return false;
	
	FMassVelocityFragment* MassVelocityFragment = EntityManager.GetFragmentDataPtr<FMassVelocityFragment>(EntityHandle);
	if (!MassVelocityFragment) return false;
	
	MassVelocityFragment->Value = FVector::ZeroVector;
	
	if (auto* LOD = EntityManager.GetFragmentDataPtr<FMassRepresentationLODFragment>(EntityHandle))
	{
		LOD->LOD = EMassLOD::Off;
		return true;
	}
	
	return false;
}


bool MassHordeUtility::ShowPassenger(const FMassEntityManager& EntityManager, const FMassEntityHandle& EntityHandle, const FVector& ShowLocation)
{
	FTransformFragment* TransformFragment =  EntityManager.GetFragmentDataPtr<FTransformFragment>(EntityHandle);
	if (!TransformFragment) return false;
	
	TransformFragment->GetMutableTransform().SetLocation(ShowLocation);
	
	if (auto* LOD = EntityManager.GetFragmentDataPtr<FMassRepresentationLODFragment>(EntityHandle))
	{
		LOD->LOD = EMassLOD::Low;
		return true;
	}
	
	return false;
}
