// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MassHordeDataPrepProcessor.generated.h"

/**
 * 
 */
UCLASS()
class MASSHORDE_API UMassHordeDataPrepProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:

	UMassHordeDataPrepProcessor();

protected:

	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

private:

	FMassEntityQuery DataPrepProcessorQuery;	
};
