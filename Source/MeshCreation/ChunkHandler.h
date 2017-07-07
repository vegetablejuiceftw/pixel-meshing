// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RuntimeMeshComponent.h"
#include "ChunkActor.h"
#include "ChunkHandler.generated.h"

UCLASS()
class MESHCREATION_API AChunkHandler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkHandler();
	~AChunkHandler();
	void Init(int dimension, int scale, FVector location);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	int16 *voxelArray;

protected:
	int DIMENSION;
	int SCALE;
	int lod;
	FVector pos;
	AChunkActor *visibleChunk;

	void GenerateVoxels();
	void DrawVoxels();
	int VOXEL_COUNT;
};
