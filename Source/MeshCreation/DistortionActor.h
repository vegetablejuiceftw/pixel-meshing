// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Texture.h"
#include "DistortionActor.generated.h"

UCLASS()
class MESHCREATION_API ADistortionActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADistortionActor();
	ADistortionActor(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "StaticMesh Components")
	UStaticMeshComponent* mesh;

	// Dynamic texture methods
	void CreateTexture();
	void UpdateTexture();
	void PaintOnTexture();

	int TEXTURE_SIZE = 128;
	int PIXEL_COUNT = TEXTURE_SIZE * TEXTURE_SIZE;
	int32 BYTES_PER_PIXEL = 4;
	int BUFFER_SIZE = PIXEL_COUNT * BYTES_PER_PIXEL;
	int BUFFER_DIMENSION = TEXTURE_SIZE * BYTES_PER_PIXEL;

	uint8 *pixelData;

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
