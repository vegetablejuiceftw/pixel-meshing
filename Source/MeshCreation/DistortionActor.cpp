#include "DistortionActor.h"
#include "ConstructorHelpers.h"
#include "Engine/Texture.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"


// Sets default values
ADistortionActor::ADistortionActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	mesh = ObjectInitializer.CreateAbstractDefaultSubobject<UStaticMeshComponent>(this, TEXT("CubeMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	mesh->SetStaticMesh(StaticMesh.Object);


	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADistortionActor::BeginPlay()
{
	Super::BeginPlay();
	CreateTexture();
}

// Called every frame
void ADistortionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PaintOnTexture();
	UpdateTexture();
}

void ADistortionActor::CreateTexture()
{
	pixelData = new uint8[BUFFER_SIZE];
	materialInstanceDynamic = mesh->CreateAndSetMaterialInstanceDynamic(0);

	// create dynamic texture
	texture = UTexture2D::CreateTransient(TEXTURE_SIZE, TEXTURE_SIZE);
	texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
//	texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	texture->Filter = TextureFilter::TF_Nearest;
	texture->SRGB = 0;
//	texture->AddToRoot();
	texture->UpdateResource();

	materialInstanceDynamic->SetTextureParameterValue(TEXTURE_PARAM_NAME, texture);
	updateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, TEXTURE_SIZE, TEXTURE_SIZE);
}

void ADistortionActor::PaintOnTexture()
{
	int colors[] = { 255 * FMath::FRand(), 255 * FMath::FRand() , 255 * FMath::FRand() , 255 * FMath::FRand() };
	auto num = FMath::RandRange(1, 113);
	auto start = FMath::RandRange(0, 4);
	auto step = FMath::RandRange(1, 5);

	for (auto i = start; i < PIXEL_COUNT; i+=step) {
		auto iBlue = i * 4 + 0;
		auto iGreen = i * 4 + 1;
		auto iRed = i * 4 + 2;
		auto iAlpha = i * 4 + 3;
		if (i % num == 0) {
			pixelData[iBlue] = colors[i % 4];
			pixelData[iGreen] = colors[(i + 1) % 4];
			pixelData[iRed] = colors[(i + 2) % 4];
			pixelData[iAlpha] = 255;
		}
		else {
			pixelData[iBlue] = 0;
			pixelData[iGreen] = 0;
			pixelData[iRed] = 0;
			pixelData[iAlpha] = 0;
		}
	}
}

void ADistortionActor::UpdateTexture()
{
	texture->UpdateTextureRegions(0, 1, updateTextureRegion, BUFFER_DIMENSION, BYTES_PER_PIXEL, pixelData);
	materialInstanceDynamic->SetTextureParameterValue(TEXTURE_PARAM_NAME, texture);
}
