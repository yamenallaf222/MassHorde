// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/Processors/MassHordeDeathProcessor.h"

#include "MassExecutionContext.h"
#include "Mass/Fragments/MassHordeFragments.h"
#include "Subsystems/MassHordeWorldSubsystem.h"


UMassHordeDeathProcessor::UMassHordeDeathProcessor() : EntityQuery(*this)
{
}

void UMassHordeDeathProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FHealthFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddTagRequirement<FMassHordeWildlingTag>(EMassFragmentPresence::All);
	
	EntityQuery.RegisterWithProcessor(*this);
}

void UMassHordeDeathProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	auto* Subsystem = Context.GetWorld()->GetSubsystem<UMassHordeWorldSubsystem>();
	
	if (!Subsystem) return;
	
	EntityQuery.ForEachEntityChunk(Context, [&](const FMassExecutionContext& SubContext)
	{
		TConstArrayView<FHealthFragment> ChunkHealthFragments = SubContext.GetFragmentView<FHealthFragment>();
		int32 NumEntities = SubContext.GetNumEntities();
		
		for (int i = 0; i < NumEntities; i++)
		{
			if (ChunkHealthFragments[i].CurrentHealth <= 0)
			{
				FMassEntityHandle EntityHandle = SubContext.GetEntity(i);
				Subsystem->EnqueueEntityToPool(EntityHandle, SubContext);
			}
		}
	});
}
