// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/Processors/MassHordeDataPrepProcessor.h"
#include "Mass/Fragments/MassHordeFragments.h" 
#include "MassExecutionContext.h"
#include "Subsystems/MassHordeWorldSubsystem.h"

UMassHordeDataPrepProcessor::UMassHordeDataPrepProcessor() : DataPrepProcessorQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::PrePhysics;
}

void UMassHordeDataPrepProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	DataPrepProcessorQuery.AddTagRequirement<FMassHordeWildlingTag>(EMassFragmentPresence::All);
	DataPrepProcessorQuery.RegisterWithProcessor(*this);
}



void UMassHordeDataPrepProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	
	UMassHordeWorldSubsystem* MassHordeWorldSubsystem =  GetWorld()->GetSubsystem<UMassHordeWorldSubsystem>();
	
	if (!MassHordeWorldSubsystem) return;
	
	auto* SharedFragment = MassHordeWorldSubsystem->GetMutableHordeSharedFragment();
	
	if (!SharedFragment) return;
	
	DataPrepProcessorQuery.ForEachEntityChunk(Context, [&](FMassExecutionContext& SubContext) {

			SharedFragment->PlayerLocation = PlayerLocation;
		});
}

