// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MassHordeMoveProcessor.generated.h"

/**
 * 
 */
UCLASS()
class MASSHORDE_API UMassHordeMoveProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
public:
	UMassHordeMoveProcessor();
	
protected:
	void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery EntityQuery;
};
