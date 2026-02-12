// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/Processors/MassHordeVisionProcessor.h"
#include "MassCommonFragments.h"
#include "Mass/Fragments/MassHordeFragments.h"
#include "MassExecutionContext.h"
#include "Data/MassHordeDeveloperSettings.h"
#include "Subsystems/MassHordeWorldSubsystem.h"
#ifdef WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif


UMassHordeVisionProcessor::UMassHordeVisionProcessor() : VisionQuery(*this)
{
	ExecutionFlags = static_cast<int32> (EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::PostPhysics;
}

void UMassHordeVisionProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	VisionQuery.AddTagRequirement<FMassHordeWildlingTag>(EMassFragmentPresence::All);
	VisionQuery.AddTagRequirement<FEntityDeadAndPooledTag>(EMassFragmentPresence::None);
	VisionQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	VisionQuery.RegisterWithProcessor(*this);
}

FORCEINLINE bool UMassHordeVisionProcessor::VisionConeCheck(const FVector& DirectionToPlayer, const FVector& EntityForward, const float& ConeHalfAngle) const
{
	if (FVector::DotProduct(DirectionToPlayer, EntityForward) >= FMath::Cos(FMath::DegreesToRadians(ConeHalfAngle)))
	{
		return true;
	}
	
	return false;
}

#ifdef WITH_EDITOR
bool UMassHordeVisionProcessor::DebuggedVisionConeCheck(const FVector& DirectionToPlayer, const FVector& EntityForward, const float& ConeHalfAngle, const FVector& DebugEntityLocation) const
{
	bool PlayerSeen = false;
	float AngleRadians = FMath::DegreesToRadians(ConeHalfAngle);
	
	if (FVector::DotProduct(DirectionToPlayer, EntityForward) >= FMath::Cos(AngleRadians))
		PlayerSeen = true;
	
	
	FColor DebugColor = !PlayerSeen ? FColor::Red : FColor::Green;
	
	/*
	 * @todo please change the hardcodded Cone Length param to Developer Settings value one
	 */
	DrawDebugCone(GetWorld(), DebugEntityLocation, EntityForward, 1000.f, /* half angle cone here*/ AngleRadians, AngleRadians,
		8, DebugColor, false, 3.f, MAX_uint8, 2.f);
	
	return PlayerSeen;
}
#endif

void UMassHordeVisionProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	
	const UMassHordeDeveloperSettings* MassHordeDeveloperSettings = GetDefault<UMassHordeDeveloperSettings>();
	if (!MassHordeDeveloperSettings) return;
	
	UMassHordeWorldSubsystem* MassHordeWorldSubsystem = Context.GetWorld()->GetSubsystem<UMassHordeWorldSubsystem>();
	if (!MassHordeWorldSubsystem) return;
	
	const FMassHordeSharedFragment* HordeSharedFragment =  MassHordeWorldSubsystem->GetHordeSharedFragment();
	if (!HordeSharedFragment) return;
	
	VisionQuery.ForEachEntityChunk(Context, [&](const FMassExecutionContext& SubContext)
		{

			TConstArrayView<FTransformFragment> TransformFragmentView = SubContext.GetFragmentView<FTransformFragment>();
			
			int32 SubContextEntityCount = SubContext.GetNumEntities();

			for (int i = 0; i < SubContextEntityCount; i++)
			{
				const FTransform EntityTransform = TransformFragmentView[i].GetTransform();

				const FVector EntityForward = EntityTransform.GetRotation().GetForwardVector().GetSafeNormal();
				const FVector EntityDirectionToPlayer = (HordeSharedFragment->PlayerLocation - EntityTransform.GetLocation()).GetSafeNormal();

				bool PlayerSeen =
#ifdef WITH_EDITOR
					DebuggedVisionConeCheck(EntityDirectionToPlayer, EntityForward, MassHordeDeveloperSettings->EntitiesConeHalfAngle, EntityTransform.GetLocation());
#else
					VisionConeCheck(EntityDirectionToPlayer, EntityForward, MassHordeDeveloperSettings->EntitiesConeHalfAngle);
#endif
	
				// it is safe to call remove the tag even if the entity does not have it as Mass Framework handles this case
				PlayerSeen ? Context.Defer().PushCommand<FMassCommandAddTag<FPlayerVisible>>(SubContext.GetEntity(i)) :
					Context.Defer().PushCommand<FMassCommandRemoveTag<FPlayerVisible>>(SubContext.GetEntity(i));
			}			
		});
}