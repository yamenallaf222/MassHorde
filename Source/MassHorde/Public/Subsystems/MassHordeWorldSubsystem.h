// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTemplate.h"
#include "Data/MassHordeDebugData.h"
#include "Subsystems/WorldSubsystem.h"
#include "MassHordeWorldSubsystem.generated.h"

/**
 *
 */

class UMassEntityConfigAsset;
struct FMassEntityHandle;
struct FMassEntityManager;

USTRUCT()
struct MASSHORDE_API FMassHordeSpawnRequest
{
	GENERATED_BODY()
	
	FTransform SpawnTransform = FTransform::Identity;
	const FMassEntityTemplate* EntityTemplate = nullptr;
	int32 RemainingCount = 0;
	
};

UCLASS()
class MASSHORDE_API UMassHordeWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	void InitTemplateConfigs();
	void InitDebugData();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	int32 RetrievePooledEntities(int32 Count, TArray<FMassEntityHandle>& Out);
	int32 GetPooledCount() const;
	int32 GetLiveCount() const;

	void SpawnRoutine();
	void InitializeSpawnRoutine();
	void InitConfigTemplates(const UWorld& World);
	const FMassEntityTemplate* GetHordeEntityTemplate() const;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;


	void UnregisterHordeEntity(const FMassEntityHandle& Entity);
	void RegisterHordeEntity(const FMassEntityHandle& Entity);
	void EnqueueEntityToPool(const FMassEntityHandle& Entity, const FMassExecutionContext& Context); 
	void EnqueueSpawn(const FMassHordeSpawnRequest& SpawnRequest);


private:
	
	void InitializeEntityManager();
	void ProcessPendingSpawns();
	void InitialSpawn() const;
	
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
