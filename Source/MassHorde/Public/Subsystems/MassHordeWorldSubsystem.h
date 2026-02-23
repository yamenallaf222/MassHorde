// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTemplate.h"
#include "Data/MassHordeDebugData.h"
#include "Subsystems/WorldSubsystem.h"
#include "Data/EssentialSystemData.h"
#include "Mass/Fragments/MassHordeFragments.h"
#include "MassHordeWorldSubsystem.generated.h"
/**
 *
 */

class UMassEntityConfigAsset;
struct FMassEntityHandle;
struct FMassEntityManager;


UCLASS()
class MASSHORDE_API UMassHordeWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	void InitTemplateConfigs();
	void InitDebugData();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	//Getters
	int32 RetrievePooledEntities(int32 Count, TArray<FMassEntityHandle>& Out);
	int32 GetPooledCount() const;
	int32 GetLiveCount() const;
	FORCEINLINE const FMassHordeSharedFragment* GetHordeSharedFragment() const;

	void SpawnRoutine();
	void InitializeSpawnRoutine();
	void InitConfigTemplates(const UWorld& World);
	const FMassEntityTemplate* GetHordeEntityTemplate() const;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	void UnregisterHordeEntity(const FMassEntityHandle& Entity);
	void RegisterHordeEntity(const FMassEntityHandle& Entity);
	void EnqueueEntityToPool(const FMassEntityHandle& Entity, const FMassExecutionContext& Context); 
	void EnqueueSpawn(const FMassHordeSpawnRequest& SpawnRequest);

	
	FORCEINLINE FMassHordeSharedFragment* GetMutableHordeSharedFragment();

private:
	
	void InitializeEntityManager();
	void ProcessPendingSpawns();
	void ConfigureNewEntities(TArray<FMassEntityHandle>& NewEntities, const bool& bDebugEnabled);
	void InitialSpawn();
	
	
	//Shared Fragment accross the horde
	FMassHordeSharedFragment* HordeSharedFragment;
	
	UPROPERTY()
	UMassEntityConfigAsset* HordeConfig = nullptr;
	
	FMassEntityTemplate HordeTemplate;
	
	FMassEntityManager* EntityManager;
	TArray<FMassHordeSpawnRequest> PendingSpawns;
	TArray<FMassEntityHandle> PooledEntities;
	TArray<FMassEntityHandle> WorldEntities;
	
	FTimerHandle SpawnTimerHandle;
	
	//debugging data
	TArray<FMassHordeEntityDebugData> HordeEntitiesSnapShot;
	int32 NextHordeEntityDebugSlot = 0;
	
};
