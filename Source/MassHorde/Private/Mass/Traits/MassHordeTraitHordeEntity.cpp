// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/Traits/MassHordeTraitHordeEntity.h"
#include "MassEntityTemplateRegistry.h"
#include "Mass/Fragments/MassHordeFragments.h"

void UMassHordeTraitHordeEntity::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddTag<FMassHordeWildlingTag>();
}
