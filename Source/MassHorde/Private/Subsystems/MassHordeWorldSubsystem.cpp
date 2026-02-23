// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/MassHordeWorldSubsystem.h"

#include "MassCommonFragments.h"
#include "MassEntityConfigAsset.h"
#include "MassEntitySubsystem.h"
#include "MassExecutionContext.h"
#include "MassSpawnerSubsystem.h"
#include "Data/MassHordeDeveloperSettings.h"
#include "Utilities/MassHordeUtility.h"
#include "NavigationSystem.h"
#ifdef WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif



void UMassHordeWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
		
	InitializeEntityManager();
	InitTemplateConfigs();
	
#if WITH_EDITOR
	InitDebugData();
#endif
}

void UMassHordeWorldSubsystem::InitTemplateConfigs()
{
	const UMassHordeDeveloperSettings* Settings = GetDefault<UMassHordeDeveloperSettings>();
	
	if (!Settings) return;
	
	if (!Settings->HordeConfig.IsNull())
	{
		HordeConfig = Settings->HordeConfig.LoadSynchronous();
	}
}

void UMassHordeWorldSubsystem::InitDebugData()
{
	const UMassHordeDeveloperSettings* Settings = GetDefault<UMassHordeDeveloperSettings>();
	
	if (!Settings) return;
	
	HordeEntitiesSnapShot.Init(FMassHordeEntityDebugData(), Settings->MaxEntityOverall);
	
}

void UMassHordeWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

int32 UMassHordeWorldSubsystem::RetrievePooledEntities(const int32 Count, TArray<FMassEntityHandle>& Out)
{
	const int32 Available = FMath::Min<int32>(Count, PooledEntities.Num());
	
	for (int32 i = 0; i < Available - 1; ++i)
	{
		FMassEntityHandle Entity = PooledEntities.Pop(EAllowShrinking::No);
		if (!Entity.IsValid()) continue;
		
		Out.Add(Entity);
	}
	
	return Available;
}

int32 UMassHordeWorldSubsystem::GetPooledCount() const
{
	return PooledEntities.Num();
}

int32 UMassHordeWorldSubsystem::GetLiveCount() const
{
	return WorldEntities.Num();
}

FORCEINLINE const FMassHordeSharedFragment* UMassHordeWorldSubsystem::GetHordeSharedFragment() const
{
	if (HordeSharedFragment)
	{
		return HordeSharedFragment;
	}
	
	return nullptr;
}	

FORCEINLINE FMassHordeSharedFragment* UMassHordeWorldSubsystem::GetMutableHordeSharedFragment()
{
	if (HordeSharedFragment)
	{
		return HordeSharedFragment;
	}
	
	return nullptr;
}

void UMassHordeWorldSubsystem::SpawnRoutine()
{
	ProcessPendingSpawns();
}

void UMassHordeWorldSubsystem::InitializeSpawnRoutine()
{
	const UWorld* World = GetWorld();
	
	if (!World) return;;
	
	if (!World->GetTimerManager().IsTimerActive(SpawnTimerHandle))
	{
		World->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ThisClass::SpawnRoutine, 0.1f, true);
	}
}

void UMassHordeWorldSubsystem::InitConfigTemplates(const UWorld& World)
{
	if (HordeConfig)
	{
		HordeTemplate = HordeConfig->GetOrCreateEntityTemplate(World);
	}
}

const FMassEntityTemplate* UMassHordeWorldSubsystem::GetHordeEntityTemplate() const
{
	return HordeTemplate.IsValid() ? &HordeTemplate : nullptr;
}

void UMassHordeWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	HordeSharedFragment = new FMassHordeSharedFragment{};
	
	InitializeSpawnRoutine();
	InitConfigTemplates(InWorld);
	
	
	//the order does matter as InitialSpawn relies on InitConfigTemplates(...)
	InitialSpawn();
	
}

void UMassHordeWorldSubsystem::UnregisterHordeEntity(const FMassEntityHandle& Entity)
{
	WorldEntities.RemoveSwap(Entity);
}

void UMassHordeWorldSubsystem::RegisterHordeEntity(const FMassEntityHandle& Entity)
{
	WorldEntities.Add(Entity);	
}

void UMassHordeWorldSubsystem::EnqueueEntityToPool(const FMassEntityHandle& Entity,
                                                   const FMassExecutionContext& Context)
{
	if (!EntityManager || !Entity.IsValid()) return;
	
	UnregisterHordeEntity(Entity);
	
	if (Context.DoesArchetypeHaveTag<FMovingTag>())
	{
		Context.Defer().PushCommand<FMassCommandRemoveTag<FMovingTag>>(Entity);
	}
	
	Context.Defer().PushCommand<FMassCommandAddTag<FEntityDeadAndPooledTag>>(Entity);
	
	
	MassHordeUtility::HidePassenger(*EntityManager, Entity);
	
	PooledEntities.Add(Entity);
	
}

void UMassHordeWorldSubsystem::EnqueueSpawn(const FMassHordeSpawnRequest& SpawnRequest)
{
	PendingSpawns.Add(SpawnRequest);
}

void UMassHordeWorldSubsystem::InitializeEntityManager()
{
	if ( auto* EntitySubsystem = GetWorld()->GetSubsystem<UMassEntitySubsystem>() )
	{
		EntityManager = &EntitySubsystem->GetMutableEntityManager();
	}
	
	check(EntityManager);
}

void UMassHordeWorldSubsystem::ProcessPendingSpawns()
{
	if ( !EntityManager || PendingSpawns.Num() == 0) return;
	
	auto* Spawner = GetWorld()->GetSubsystem<UMassSpawnerSubsystem>();
	auto* MassEntitySubsystem = GetWorld()->GetSubsystem<UMassEntitySubsystem>();
	const auto* Settings = GetDefault<UMassHordeDeveloperSettings>();
	
	if (!Settings) return;
	
	bool bDebugEnabled = Settings->bGlobalMassSystemDebugging & Settings->bDebugSpawnPosition;
	
	if (!Spawner || !MassEntitySubsystem) return;
	int32 Budget = Settings->MaxSpawnsPerFrame;
	
	
	for (int i = PendingSpawns.Num() - 1; i >= 0 && Budget > 0; --i)
	{
		FMassHordeSpawnRequest& SpawnRequest = PendingSpawns[i];
		const int32 ThisBatch = FMath::Min<int32>(Budget,SpawnRequest.RemainingCount);
		
		TArray<FMassEntityHandle> NewEntities;
		const int32 Reused = RetrievePooledEntities(ThisBatch, NewEntities);
		const int32 Need = ThisBatch - Reused;

		
		if (Reused < ThisBatch)
		{
			if (GetLiveCount() + Need > Settings->MaxLiveSpawnsCount)
				return;

			TArray<FMassEntityHandle> Spawned;
			
			Spawner->SpawnEntities(*SpawnRequest.EntityTemplate, Need, Spawned);

			NewEntities.Append(Spawned);
			WorldEntities.Append(Spawned);
		}
		
		
		SpawnRequest.RemainingCount -= ThisBatch;
		Budget -= ThisBatch;
		
		if (SpawnRequest.RemainingCount <= 0)
		{
			PendingSpawns.RemoveAtSwap(i);
		}

		
		ConfigureNewEntities(NewEntities, bDebugEnabled);

	}
	
}

void UMassHordeWorldSubsystem::ConfigureNewEntities(TArray<FMassEntityHandle>& NewEntities, const bool& bDebugEnabled)
{
	for (int j = 0; j < NewEntities.Num(); ++j)
	{
		if (FTransformFragment* TransformFragment = EntityManager->GetFragmentDataPtr<FTransformFragment>(NewEntities[j]))
		{
			FVector RandomLocation;
			if (UNavigationSystemV1::K2_GetRandomReachablePointInRadius(GetWorld(), FVector::ZeroVector, RandomLocation, 1000.f))
			{
				RandomLocation.Z += 200.f;
#ifdef WITH_EDITOR
				if (bDebugEnabled) DrawDebugPoint(GetWorld(), RandomLocation, 10.f, FColor::Red, true, -1.0f, MAX_uint8);
#endif
				TransformFragment->GetMutableTransform().SetLocation(RandomLocation);
			}
		}

		if (FHealthFragment* HealthFragment = EntityManager->GetFragmentDataPtr<FHealthFragment>(NewEntities[j]))
		{
			HealthFragment->CurrentHealth = 100.f;
		}
	}
}

void UMassHordeWorldSubsystem::InitialSpawn()
{
	auto* Spawner = GetWorld()->GetSubsystem<UMassSpawnerSubsystem>();
	const auto* Settings = GetDefault<UMassHordeDeveloperSettings>();
	if (!Settings) return;
	
	TArray<FMassEntityHandle> NewEntities;
	Spawner->SpawnEntities(HordeTemplate, 1, NewEntities);
	
	
	ConfigureNewEntities(NewEntities, Settings->bGlobalMassSystemDebugging & Settings->bDebugSpawnPosition);
	
}