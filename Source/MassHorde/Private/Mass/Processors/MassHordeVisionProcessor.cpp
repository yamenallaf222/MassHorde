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
	bool bPlayerSeen = false;
	float AngleRadians = FMath::DegreesToRadians(ConeHalfAngle);
	
	if (FVector::DotProduct(DirectionToPlayer, EntityForward) >= FMath::Cos(AngleRadians))
		bPlayerSeen = true;
	
	
	FColor DebugColor = !bPlayerSeen ? FColor::Red : FColor::Green;
	
	/*
	 * @todo please change the hardcodded Cone Length param to Developer Settings value one
	 */
	DrawDebugCone(GetWorld(), DebugEntityLocation, EntityForward, 1000.f, /* half angle cone here*/ AngleRadians, AngleRadians,
		8, DebugColor, false, 3.f, MAX_uint8, 2.f);
	
	GEngine->AddOnScreenDebugMessage(0, 2.f, bPlayerSeen ? FColor::Green : FColor::Red, bPlayerSeen ? TEXT("Player Seen") : TEXT("Player is not seen"));
	
	return bPlayerSeen;
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
				const FTransform& EntityTransform = TransformFragmentView[i].GetTransform();

				const FVector& EntityForward = EntityTransform.GetRotation().GetForwardVector().GetSafeNormal();
				const FVector& EntityDirectionToPlayer = (HordeSharedFragment->PlayerLocation - EntityTransform.GetLocation()).GetSafeNormal();

				bool bPlayerSeen =
#ifdef WITH_EDITOR
					MassHordeDeveloperSettings->bDebugVisionCones ? DebuggedVisionConeCheck(EntityDirectionToPlayer, EntityForward,
						MassHordeDeveloperSettings->EntitiesConeHalfAngle, EntityTransform.GetLocation()) : VisionConeCheck(EntityDirectionToPlayer, EntityForward, MassHordeDeveloperSettings->EntitiesConeHalfAngle); 
#else
					VisionConeCheck(EntityDirectionToPlayer, EntityForward, MassHordeDeveloperSettings->EntitiesConeHalfAngle);
#endif
	
				// it is safe to call remove the tag even if the entity does not have it as Mass Framework handles this case
				bPlayerSeen ? Context.Defer().PushCommand<FMassCommandAddTag<FPlayerVisible>>(SubContext.GetEntity(i)) :
					Context.Defer().PushCommand<FMassCommandRemoveTag<FPlayerVisible>>(SubContext.GetEntity(i));
			}			
		});
}