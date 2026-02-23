// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MassHordeMovementDataPropagatorProcessor.generated.h"

/**
 * 
 */
UCLASS()
class MASSHORDE_API UMassHordeMovementDataPropagatorProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
public:
	UMassHordeMovementDataPropagatorProcessor();
	
protected:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
	
private:
	FMassEntityQuery MovementDataPropagatorQuery;
};
