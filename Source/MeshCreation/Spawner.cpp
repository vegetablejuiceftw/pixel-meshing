// Fill out your copyright notice in the Description page of Project Settings.

#include "Spawner.h"
#include "ChunkHandler.h"
#include "UtilityTimer.h"


ASpawner::ASpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	auto timer = UtilityTimer();

	FVector Location(0.0f, 0.0f, 0.0f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);

	auto dimension = 48, scale = 100;

	auto count = 30;
	auto chunks = 0;
	for (auto x = -count; x < count+1; x++)
	{
		for (auto y = -count; y < count+1; y++)
		{
			for (auto z = -1; z < 1; z++)
			{
				if (FVector(x, y, z).Size() > count) continue;
				Location = FVector(x, y, z) * dimension * scale;

				auto SpawnTransform = FTransform(Rotation, Location);

				auto MyDeferredActor = Cast<AChunkHandler>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChunkHandler::StaticClass(), SpawnTransform));
				if (MyDeferredActor != nullptr)
				{
					MyDeferredActor->Init(dimension, scale, Location);
					UGameplayStatics::FinishSpawningActor(MyDeferredActor, SpawnTransform);
				}
				chunks++;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("SpawnActor row %d"), chunks);
	}
	UE_LOG(LogTemp, Warning, TEXT("SpawnActor %d"), chunks);

	timer.log();
}

void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
