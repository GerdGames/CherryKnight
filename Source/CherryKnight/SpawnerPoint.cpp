// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnerPoint.h"

// Sets default values
ASpawnerPoint::ASpawnerPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawnerPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnerPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnerPoint::SpawnEnemy()
{

}

