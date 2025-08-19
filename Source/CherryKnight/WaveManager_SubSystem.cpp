// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveManager_Subsystem.h"
#include "Spawner_Interface.h"

void UWaveManager_Subsystem::SetupAndSpawnFirstWave(int startingSpawnTokens, float spawnTokenMultiplier, float percentKillsForWave, int maxEnemies)
{
	waveNumber = 1;
	totalEnemiesSpawned = 0;
	totalEnemiesKilled = 0;
	enemiesSpawnedSinceLastWave = 0;
	enemiesKilledSinceLastWave = 0;

	maxActiveEnemies = maxEnemies;
	spawnTokens = startingSpawnTokens;
	nextWaveSpawnTokenMultiplier = spawnTokenMultiplier;
	percentKillsForNextWave = percentKillsForWave;
	SpawnWave();
}

void UWaveManager_Subsystem::SpawnWave()
{
	enemiesSpawnedSinceLastWave = 0;
	enemiesKilledSinceLastWave = 0;
	availableTokens += spawnTokens;

	waveNumber++;

	SpawnEnemies();
}

void UWaveManager_Subsystem::SpawnEnemies()
{
	if (spawnerPoints.Num() > 0)
	{
		while ((availableTokens > 0) && (activeEnemies.Num() < maxActiveEnemies))
		{
			AActor* nextSpawner = spawnerPoints[(totalEnemiesSpawned % spawnerPoints.Num())];
			if (nextSpawner && nextSpawner->Implements<USpawner_Interface>())
			{
				int nextEnemyCost = ISpawner_Interface::Execute_SpawnEnemy(nextSpawner, availableTokens);
				availableTokens -= nextEnemyCost;
				enemiesSpawnedSinceLastWave++;
				totalEnemiesSpawned++;
			}
		}
	}
}

void UWaveManager_Subsystem::IncreaseSpawnTokens()
{
	spawnTokens = floor(spawnTokens * nextWaveSpawnTokenMultiplier);
}

void UWaveManager_Subsystem::StartNextWave()
{
	SpawnWave();

	IncreaseSpawnTokens();
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

bool UWaveManager_Subsystem::AddActiveEnemy(AActor* Enemy)
{
	if (activeEnemies.AddUnique(Enemy) == INDEX_NONE)
	{
		return false;
	}

	return true;
}

bool UWaveManager_Subsystem::RemoveActiveEnemy(AActor* Enemy)
{
	if (activeEnemies.Remove(Enemy) == 0)
	{
		return false;
	}

	totalEnemiesKilled++;
	enemiesKilledSinceLastWave++;

	//If percent of enemies killed since last wave is reached, and if there are very few available tokens spawn the next wave, and we aren't already trying to spawn the next wave
	if ((enemiesKilledSinceLastWave >= (enemiesSpawnedSinceLastWave * percentKillsForNextWave)) && (availableTokens < 10) && !(GetWorld()->GetTimerManager().IsTimerActive(SpawnDelayTimer)))
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnDelayTimer, this, &UWaveManager_Subsystem::StartNextWave, 1.0f, false, 1.0f);
	}
	else if(availableTokens > 0)
	{
		SpawnEnemies();
	}

	return true;
}
