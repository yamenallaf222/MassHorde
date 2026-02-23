// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/Processors/MassHordeMovementDataPropagatorProcessor.h"

#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassCommonFragments.h"
#include "MassActorSubsystem.h"
#include "MassRepresentationFragments.h"
#include "Mass/Fragments/MassHordeFragments.h"
#include "Data/MassHordeDeveloperSettings.h"
#include "Subsystems/MassHordeWorldSubsystem.h"
#ifdef WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif

UMassHordeMovementDataPropagatorProcessor::UMassHordeMovementDataPropagatorProcessor() : MovementDataPropagatorQuery(*this)
{
	ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::PrePhysics;
	ExecutionOrder.ExecuteAfter.Add(TEXT("MassHordeDataPrepProcessor"));
	ExecutionOrder.ExecuteBefore.Add(TEXT("MassApplyForceProcessor"));
	ExecutionOrder.ExecuteBefore.Add(TEXT("MassApplyMovementProcessor"));
	
}

void UMassHordeMovementDataPropagatorProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	// ReadWrite access for modifying force and velocity
	MovementDataPropagatorQuery.AddRequirement<FMassForceFragment>(EMassFragmentAccess::ReadWrite);
	MovementDataPropagatorQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
	
	// ReadOnly access for position data - no mutations, thread-safe
	MovementDataPropagatorQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	
	// Optional: Actor fragment and representation for debugging
	MovementDataPropagatorQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
	MovementDataPropagatorQuery.AddRequirement<FMassRepresentationFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
	
	// Tag requirements for filtering entities
	MovementDataPropagatorQuery.AddTagRequirement<FMassHordeWildlingTag>(EMassFragmentPresence::All);
	MovementDataPropagatorQuery.AddTagRequirement<FEntityDeadAndPooledTag>(EMassFragmentPresence::None);
	MovementDataPropagatorQuery.AddTagRequirement<FPlayerVisible>(EMassFragmentPresence::All);
	
	MovementDataPropagatorQuery.RegisterWithProcessor(*this);
}
	
void UMassHordeMovementDataPropagatorProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	const UMassHordeDeveloperSettings* Settings = GetDefault<UMassHordeDeveloperSettings>();
	if (!Settings) return;
	
	UMassHordeWorldSubsystem* MassHordeWorldSubsystem = Context.GetWorld()->GetSubsystem<UMassHordeWorldSubsystem>();
	if (!MassHordeWorldSubsystem) return;
	
	const FMassHordeSharedFragment* HordeSharedFragment = MassHordeWorldSubsystem->GetHordeSharedFragment();
	if (!HordeSharedFragment) return;
	
	// Cache player location - const reference, thread-safe to read
	const FVector& PlayerLocation = HordeSharedFragment->PlayerLocation;
	
	// Cache settings values for performance - avoids repeated property access in loop
	const float MaxSpeed = Settings->EntityMaxSpeed;
	const float Acceleration = Settings->EntityAcceleration;
	const float StoppingDistance = Settings->EntityStoppingDistance;
	const float StoppingDistanceSq = StoppingDistance * StoppingDistance; // Pre-calculate for performance
	
#ifdef WITH_EDITOR
	const bool bDebugMovement = Settings->bDebugMovementVectors;
	UWorld* World = GetWorld();
#endif
	
	// ForEachEntityChunk processes chunks in parallel - each chunk is independent
	// This is the key to Mass Framework's performance
	MovementDataPropagatorQuery.ForEachEntityChunk(Context, [&](FMassExecutionContext& SubContext)
	{
		// Get mutable views for data we need to modify
		TArrayView<FMassForceFragment> ForceFragments = SubContext.GetMutableFragmentView<FMassForceFragment>();
		TArrayView<FMassVelocityFragment> VelocityFragments = SubContext.GetMutableFragmentView<FMassVelocityFragment>();
		
		// Get read-only view for position data - const ensures no accidental modifications
		TConstArrayView<FTransformFragment> TransformFragments = SubContext.GetFragmentView<FTransformFragment>();
		
		const int32 NumEntities = SubContext.GetNumEntities();
		
		// Process all entities in this chunk - linear iteration is cache-friendly
		for (int32 i = 0; i < NumEntities; ++i)
		{
			const FVector& EntityLocation = TransformFragments[i].GetTransform().GetLocation();
			const FVector& CurrentVelocity = VelocityFragments[i].Value;
			
			// Calculate direction to player
			const FVector DirectionToPlayer = (PlayerLocation - EntityLocation);
			const float DistanceToPlayerSq = DirectionToPlayer.SizeSquared();
			
			// Early out if already at stopping distance - avoid unnecessary calculations
			if (DistanceToPlayerSq <= StoppingDistanceSq)
			{
				// Apply deceleration force to slow down
				ForceFragments[i].Value = -CurrentVelocity * Acceleration * 0.5f;
				continue;
			}
			
			const FVector DirectionNormalized = DirectionToPlayer.GetSafeNormal();
			
			// Calculate desired velocity toward player
			const FVector DesiredVelocity = DirectionNormalized * MaxSpeed;
			
			// Calculate steering force: (Desired Velocity - Current Velocity) * Acceleration
			// This creates smooth acceleration and turning behavior
			const FVector SteeringForce = (DesiredVelocity - CurrentVelocity) * Acceleration;
			
			// Apply the force - Mass Framework will handle integration via MassApplyForceProcessor
			ForceFragments[i].Value = SteeringForce;
			
#ifdef WITH_EDITOR
			// Debug visualization - only in editor builds
			if (bDebugMovement && World)
			{
				// Draw line to player
				DrawDebugDirectionalArrow(World, EntityLocation, PlayerLocation, 50.f, FColor::Yellow, false, 0.1f, 0, 2.f);
				
				// Draw velocity vector
				DrawDebugDirectionalArrow(World, EntityLocation, EntityLocation + CurrentVelocity, 30.f, FColor::Green, false, 0.1f, 0, 2.f);
				
				// Draw desired velocity
				DrawDebugDirectionalArrow(World, EntityLocation, EntityLocation + DesiredVelocity, 30.f, FColor::Cyan, false, 0.1f, 0, 1.f);
			}
#endif
		}
	});
}
