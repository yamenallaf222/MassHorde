// Fill out your copyright notice in the Description page of Project Settings.

#include "Mass/Processors/MassHordeSpawnProcessor.h"

#include "Data/MassHordeDeveloperSettings.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "Mass/Fragments/MassHordeFragments.h"
#include "Subsystems/MassHordeWorldSubsystem.h"

UMassHordeSpawnProcessor::UMassHordeSpawnProcessor() : SpawnEntityQuery(*this)
{
    ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
    ProcessingPhase = EMassProcessingPhase::FrameEnd;
    ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Tasks;
}

void UMassHordeSpawnProcessor::ConfigureQueries(
    const TSharedRef<FMassEntityManager> &EntityManager)
{
    SpawnEntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    SpawnEntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
    SpawnEntityQuery.AddTagRequirement<FMassHordeWildlingTag>(EMassFragmentPresence::All);
    SpawnEntityQuery.RegisterWithProcessor(*this);
}

void UMassHordeSpawnProcessor::Execute(FMassEntityManager &EntityManager,
                                       FMassExecutionContext &Context)
{

    const auto *Settings = GetDefault<UMassHordeDeveloperSettings>();
    auto *Subsystem = Context.GetWorld()->GetSubsystem<UMassHordeWorldSubsystem>();
    if (!Settings)
        return;
    if (!Subsystem)
        return;
    FMassHordeSpawnRequest SpawnRequest;

    const FMassEntityTemplate *HordeTemplate = Subsystem->GetHordeEntityTemplate();

    SpawnAccumulator += Context.GetDeltaTimeSeconds();
    if (SpawnAccumulator < Settings->SpawnIntervalSeconds)
        return;
    SpawnAccumulator = 0.f;

    if (!HordeTemplate && !HordeTemplate->IsValid())
        return;

    SpawnRequest.EntityTemplate = HordeTemplate;
    SpawnRequest.RemainingCount = 1;
    // SpawnRequest.SpawnTransform = ;

    if (Subsystem->GetLiveCount() + SpawnRequest.RemainingCount > Settings->MaxLiveSpawnsCount)
        return;

    Subsystem->EnqueueSpawn(SpawnRequest);

}
