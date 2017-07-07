// Fill out your copyright notice in the Description page of Project Settings.

#include "ChunkHandler.h"
#include "ChunkActor.h"

AChunkHandler::AChunkHandler()
{
	PrimaryActorTick.bCanEverTick = true;
}

AChunkHandler::~AChunkHandler()
{
	delete voxelArray;
}

void AChunkHandler::Init(int dimension, int scale, FVector location)
{
	DIMENSION = dimension;
	SCALE = scale;
	lod = -1;
	pos = location;
}

void AChunkHandler::GenerateVoxels()
{
	int height = pos.Z / SCALE;
	int X = pos.X / SCALE;
	int Y = pos.Y / SCALE;
	float factor = 0; // pos.Size() / SCALE / DIMENSION / 3;
	auto amplitude = DIMENSION;

	for (auto x = 0; x < DIMENSION; x++)
	{
		auto xH = FMath::Sin((x + X) / 13.f);
		for (auto y = 0; y < DIMENSION; y++)
		{
			auto yH = FMath::Sin((y + Y) / 17.f);
			int xHi = (xH * 0.95 - 1.05 + factor) * amplitude, yHi = (yH * 0.95 - 1.05 + factor) * amplitude;
			int maxZ = (xHi + yHi) / 4;
			if (xH >= 0.96 && yH >= 0.96)
			{
				maxZ += 1000;
			}
			
			maxZ = FMath::Min(maxZ - height, DIMENSION - 1);		
			if (maxZ >= 0 && maxZ < DIMENSION) {
				voxelArray[x + y * DIMENSION + maxZ * DIMENSION * DIMENSION] = 1;
			}
			for (auto z = maxZ - 1; z >= 0; z--)
			{
				voxelArray[x + y * DIMENSION + z * DIMENSION * DIMENSION] = 2;
			}
		}
	}
}

void AChunkHandler::DrawVoxels()
{
	auto dimension = DIMENSION, scale = SCALE;

	auto playerLoc = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
	int currentLod = (playerLoc - pos).Size() / (SCALE * DIMENSION);
	auto level = 20;
	if (currentLod < level)
		currentLod = 0;
	else if (currentLod < level + 6)
		currentLod = 1;
	else if (currentLod < level + 10)
		currentLod = 2;
	else if (currentLod < level + 12)
		currentLod = 3;
	else
		currentLod = 4;

	if (currentLod != lod)
	{
		if (lod != -1)
		{
			visibleChunk->Destroy();
		}
		lod = currentLod;
		auto divider = FMath::Min(FMath::Pow(2, lod), dimension / 4.f);

		dimension /= divider;
		scale *= divider;

		auto SpawnTransform = FTransform(FRotator(0.0f, 0.0f, 0.0f), pos);;

		auto chunk = Cast<AChunkActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChunkActor::StaticClass(), SpawnTransform));
		if (chunk != nullptr)
		{
			chunk->Init(dimension, scale, divider, voxelArray);
			UGameplayStatics::FinishSpawningActor(chunk, SpawnTransform);
		}
		visibleChunk = chunk;
	}
}

void AChunkHandler::BeginPlay()
{
	Super::BeginPlay();

	VOXEL_COUNT = DIMENSION * DIMENSION * DIMENSION;
	voxelArray = new int16[VOXEL_COUNT];
	memset(voxelArray, 0, VOXEL_COUNT * sizeof(*voxelArray));

	GenerateVoxels();
}

void AChunkHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (FMath::RandRange(0, 32) == 0)
		DrawVoxels();	
}
