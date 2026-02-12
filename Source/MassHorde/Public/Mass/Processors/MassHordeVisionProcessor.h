// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MassHordeVisionProcessor.generated.h"

/**
 * 
 */
UCLASS()
class MASSHORDE_API UMassHordeVisionProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	
	UMassHordeVisionProcessor();

protected:

	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:

	//Helpers
	/**
	* @param DirectionToPlayer should be a safely normalized vector
	  @param EntityForward should be a safely normalized vector
	  @param ConeHalfAngle
	  @param DebugEntityLocation Only exists if the runtime is within the Editor with WITH_EDITOR defined
	  @param 
	*/
	bool VisionConeCheck(const FVector& DirectionToPlayer, const FVector& EntityForward, const float& ConeHalfAngle) const;
#ifdef WITH_EDITOR
	bool DebuggedVisionConeCheck(const FVector& DirectionToPlayer, const FVector& EntityForward, const float& ConeHalfAngle, const FVector&
	                             DebugEntityLocation) const;
#endif


	FMassEntityQuery VisionQuery;
	
};
