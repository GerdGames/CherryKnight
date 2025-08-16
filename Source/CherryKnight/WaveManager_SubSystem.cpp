// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveManager_Subsystem.h"
#include "Spawner_Interface.h"

bool UWaveManager_Subsystem::SpawnWave()
{
	int waveTokens = spawnTokens;
	int spawnedEnemies = 0;

	while (waveTokens > 0)
	{
		AActor* nextSpawner = spawnerPoints[(spawnedEnemies % spawnerPoints.Num())];
		if(nextSpawner && nextSpawner->Implements<USpawner_Interface>())
		{
			int nextEnemyCost = ISpawner_Interface::Execute_SpawnEnemy(nextSpawner, waveTokens);
			waveTokens -= nextEnemyCost;
		}

		spawnedEnemies++;
	}

	return true;
}

void UWaveManager_Subsystem::IncreaseSpawnTokens()
{
	spawnTokens = floor(spawnTokens * spawnTokenMultiplier);
}

int UWaveManager_Subsystem::GetWaveNumber()
{
	return waveNumber;
}

bool UWaveManager_Subsystem::AddSpawnerPoint(AActor* SpawnerPoint)
{
	if (SpawnerPoint && SpawnerPoint->Implements<USpawner_Interface>())
	{
		spawnerPoints.Add(SpawnerPoint);
		return true;
	}
	return false;
}
