// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/Processors/MassHordeMoveProcessor.h"

#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "Dataflow/DataflowSelection.h"
#include "Mass/Fragments/MassHordeFragments.h"
#include "Subsystems/MassHordeWorldSubsystem.h"

UMassHordeMoveProcessor::UMassHordeMoveProcessor() : EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::PrePhysics;
	ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Movement;
}

void UMassHordeMoveProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddTagRequirement<FMassHordeWildlingTag>(EMassFragmentPresence::All);
	EntityQuery.AddTagRequirement<FMovingTag>(EMassFragmentPresence::All);
	
	EntityQuery.RegisterWithProcessor(*this);
	ProcessorRequirements.AddSubsystemRequirement<UMassHordeWorldSubsystem>(EMassFragmentAccess::ReadWrite);
}

void UMassHordeMoveProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [&](FMassExecutionContext& SubContext)
	{
		TArrayView<FTransformFragment> Transforms = SubContext.GetMutableFragmentView<FTransformFragment>();
		TArrayView<FMassVelocityFragment> Velocities = SubContext.GetMutableFragmentView<FMassVelocityFragment>();
		
		for (int i = 0; i < Transforms.Num(); i++)
		{
			FTransformFragment TransformFragment = Transforms[i];
			FTransform Transfrom = TransformFragment.GetMutableTransform();
			FMassVelocityFragment Velocity = Velocities[i];
			Transfrom.SetLocation(Transfrom.GetLocation() + Velocity.Value * SubContext.GetDeltaTimeSeconds());
		}
	});
	
}

