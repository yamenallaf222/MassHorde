// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "MassHordeDeveloperSettings.generated.h"

class UMassEntityConfigAsset;

/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Mass Horde"))
class MASSHORDE_API UMassHordeDeveloperSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Config, Category="Spawning", meta=(ClampMin="1"))
	int32 MaxSpawnsPerFrame = 64;
	
	UPROPERTY(EditDefaultsOnly, Config, Category="Spawning", meta=(ClampMin="1"))
	int32 MaxLiveSpawnsCount = 64;
	
	UPROPERTY(EditDefaultsOnly, Config, Category="Entity Templates")
	TSoftObjectPtr<UMassEntityConfigAsset> HordeConfig = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Config, Category="Simulation Settings", meta=(ClampMin="1"))
	int32 MaxEntityOverall = 500;
	
	UPROPERTY(EditDefaultsOnly, Config, Category="Simulation Settings", meta=(ClampMin="1"))
	float SpawnIntervalSeconds = 0.25f;
	
	
};
