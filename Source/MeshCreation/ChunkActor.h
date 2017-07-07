// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RuntimeMeshComponent.h"
#include "ChunkActor.generated.h"


UCLASS()
class MESHCREATION_API AChunkActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AChunkActor();
	~AChunkActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Parameters")
	UMaterialInterface* Material;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Default)
	USceneComponent* RootNode;

	UPROPERTY()
	URuntimeMeshComponent* MeshComponent;

	void Init(int dimension, int scale, int factor, int16(*voxelArray));
	void Tick(float DeltaTime) override;
	void BeginPlay() override;

protected:
	void GenerateVoxelLayers();

	void CreateTexture();

	void createPoligon(FVector start, FVector planeSize, FVector2D uvStart, FVector2D uvSize, FVector normal, FRuntimeMeshTangent tangent);
	void GenerateMesh();

	void UpdateTexture() const;
	void PaintOnTexture();
	void PaintPixel(FVector2D pixel, FVector4 color) const;
	void DrawPosition(FIntVector pos, FVector4 color);
	
	int32 BYTES_PER_PIXEL = 4;

	int DIMENSION;
	int LAYERS;
	int TILE_DIMENSION;
	int SCALE;
	int SIZE;
	
	int16 *layers;
	int16 *voxelArray;

	TArray<FRuntimeMeshVertexSimple> Vertices;
	TArray<int32> Triangles;
	FBox BoundingBox;

	int textureSizeX, textureSizeY;
	int activeLayerCount;
	int pixelCount, bufferSize, bufferPitch;

	FVector2D uvSize;

	uint8 *pixelData;
	TArray<FVector2D> pixelTiles;

	// dynamic material
	UPROPERTY()
	UMaterialInstanceDynamic* materialInstanceDynamic;

	// dynamic texture
	UPROPERTY()
	UTexture2D* texture;

	FUpdateTextureRegion2D* updateTextureRegion;

	// Param name in the material view for the texture to parameter converted node
	FName TEXTURE_PARAM_NAME = FName("DynamicTextureParam");
};

// 1024: 12 576 * 68 ** 3 -> 2GB 100fps 181M 1632D
// 512: 23 2304 * 42 ** 3 -> 2GB 100fps  170M 1974D
// 256: 3600 * 24 ** 3 -> .6GB 70fps  49M 1440D
// 128: 6400 * 16 ** 3 -> .2GB 40fps  26M 1280D

// 50 ** 3 * 1089 = 136M 100fps 1G 800D 4S
// 64 ** 3 * 625 = 164M 90fps 1.3G 768D 6s

// 70 ** 3 * 625 = 214M 80fps 1.7G 840D 6s
// 80 ** 3 * 529 = 270M 60fps 2.2G 880D 7s
// 80 ** 3 * 361 = 185M 60fps 1.5G 720D 4.2S