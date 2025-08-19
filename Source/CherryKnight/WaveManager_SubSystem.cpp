// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveManager_Subsystem.h"
#include "Spawner_Interface.h"

bool UWaveManager_Subsystem::SpawnWave()
{
	int waveTokens = spawnTokens;
	enemiesSpawnedByLastWave = 0;
	enemiesKilledSinceLastWave = 0;

	waveNumber++;

	if (spawnerPoints.Num() > 0)
	{
		while (waveTokens > 0)
		{
			AActor* nextSpawner = spawnerPoints[(enemiesSpawnedByLastWave % spawnerPoints.Num())];
			if (nextSpawner && nextSpawner->Implements<USpawner_Interface>())
			{
				int nextEnemyCost = ISpawner_Interface::Execute_SpawnEnemy(nextSpawner, waveTokens);
				waveTokens -= nextEnemyCost;
				enemiesSpawnedByLastWave++;
			}
			else
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

void UWaveManager_Subsystem::IncreaseSpawnTokens()
{
	spawnTokens = floor(spawnTokens * spawnTokenMultiplier);
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

	enemiesKilledSinceLastWave++;

	if (enemiesKilledSinceLastWave >= (enemiesSpawnedByLastWave * killsForNextWavePercentage))
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnDelayTimer, this, &UWaveManager_Subsystem::StartNextWave, 1.0f, false, 1.0f);
	}

	return true;
}
