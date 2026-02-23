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
	
	UPROPERTY(EditDefaultsOnly, Config, Category="Simulation Settings", meta=(ClampMin="1", ClampMax="90"))
	float EntitiesConeHalfAngle = 45.f;
	
	UPROPERTY(EditDefaultsOnly, Config, Category="Debugging Settings")
	bool bGlobalMassSystemDebugging = true;
	
	UPROPERTY(EditDefaultsOnly, Config, Category="Debugging Settings")
	bool bDebugVisionCones = true;
	
	UPROPERTY(EditDefaultsOnly, Config, Category="Debugging Settings")
	bool bDebugSpawnPosition = true;
	
	UPROPERTY(EditDefaultsOnly, Config, Category="Debugging Settings")
	bool bDebugMovementVectors = true;
	
	// Movement Settings
	UPROPERTY(EditDefaultsOnly, Config, Category="Movement Settings", meta=(ClampMin="0.0"))
	float EntityMaxSpeed = 400.f;
	
	UPROPERTY(EditDefaultsOnly, Config, Category="Movement Settings", meta=(ClampMin="0.0"))
	float EntityAcceleration = 800.f;
	
	UPROPERTY(EditDefaultsOnly, Config, Category="Movement Settings", meta=(ClampMin="0.0"))
	float EntityStoppingDistance = 150.f;
	
	UPROPERTY(EditDefaultsOnly, Config, Category="Movement Settings", meta=(ClampMin="0.0"))
	float EntityTurnSpeed = 5.f;
	
};
