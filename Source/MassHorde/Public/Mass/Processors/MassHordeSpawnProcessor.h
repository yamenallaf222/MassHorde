// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MassHordeSpawnProcessor.generated.h"

/**
 *
 */
UCLASS()
class MASSHORDE_API UMassHordeSpawnProcessor : public UMassProcessor
{
    GENERATED_BODY()

public:
    UMassHordeSpawnProcessor();

protected:
    
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager> &EntityManager) override;
    virtual void Execute(FMassEntityManager &EntityManager,
                         FMassExecutionContext &Context) override;

private:
    float SpawnAccumulator = 0;
    FMassEntityQuery SpawnEntityQuery;
};
