// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WaveManager_Subsystem.generated.h"

/**
 * 
 */
UCLASS()
class CHERRYKNIGHT_API UWaveManager_Subsystem : public UWorldSubsystem
{
	GENERATED_BODY()
private:
	int waveNumber = 1;
	int spawnTokens = 25;
	int enemiesSpawnedByLastWave = 0;
	int enemiesKilledSinceLastWave = 0;
	float spawnTokenMultiplier = 1.1;
	float killsForNextWavePercentage = 0.75;
	TArray<AActor*> spawnerPoints;
	TArray<AActor*> activeEnemies;

public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Spawn Enemies For Wave"), Category = "Wave Management")
	bool SpawnWave();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Increase Spawn Tokens"), Category = "Wave Management")
	void IncreaseSpawnTokens();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Start Next Wave"), Category = "Wave Management")
	void StartNextWave();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Wave Number"), Category = "Wave Management")
	int GetWaveNumber();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Spawner Point"), Category = "Wave Management")
	bool AddSpawnerPoint(AActor* SpawnerPoint);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Active Enemy"), Category = "Wave Management")
	bool AddActiveEnemy(AActor* Enemy);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Remove Active Enemy"), Category = "Wave Management")
	bool RemoveActiveEnemy(AActor* Enemy);
};
