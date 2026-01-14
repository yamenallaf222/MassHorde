// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MassHordeDeathProcessor.generated.h"

/**
 * 
 */
UCLASS()
class MASSHORDE_API UMassHordeDeathProcessor : public UMassProcessor
{
	GENERATED_BODY()
public:
	
	UMassHordeDeathProcessor();
	
protected:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
	
private:
	FMassEntityQuery EntityQuery;
	
};
