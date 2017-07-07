#include "ChunkActor.h"
#include "UtilityTimer.h"

AChunkActor::AChunkActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootNode = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = RootNode;

	MeshComponent = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("ProceduralMesh"));
	MeshComponent->bShouldSerializeMeshData = false;
	MeshComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> mat(TEXT("Material'/Game/ChangeAble.ChangeAble'"));
	Material = mat.Object;
	MeshComponent->SetMaterial(0, mat.Object);
}

AChunkActor::~AChunkActor()
{
	delete pixelData;
	delete updateTextureRegion;
	delete layers;
	delete voxelArray;
}

void AChunkActor::Init(int dimension, int scale, int factor, int16 (*voxels))
{
	DIMENSION = dimension;
	LAYERS = DIMENSION + 1;
	TILE_DIMENSION = DIMENSION + 1;
	SCALE = scale;
	SIZE = DIMENSION * SCALE;
	BYTES_PER_PIXEL = 4;

	layers = new int16[LAYERS * 3];

	for (auto i = 0; i < LAYERS * 3; i++) {
		layers[i] = 0;
	}
	voxelArray = new int16[DIMENSION * DIMENSION * DIMENSION];
	auto actualDimension = DIMENSION * factor;	

	for (auto x = 0; x < DIMENSION; x++)
	{
		for (auto y = 0; y < DIMENSION; y++)
		{
			for (auto z = 0; z < DIMENSION; z++)
			{
				auto current = x + y * DIMENSION + z * DIMENSION * DIMENSION;
				if (factor == 1)
				{
					voxelArray[current] = voxels[current];
				} else {
					auto r = factor - 1;
					auto actual = (x * factor + FMath::RandRange(0, r))  + (y * factor + FMath::RandRange(0, r)) * actualDimension + (z * factor + FMath::RandRange(0, r)) * actualDimension * actualDimension;
					voxelArray[current] = voxels[actual];
				}
			}
		}
	}
}

void AChunkActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AChunkActor::BeginPlay()
{	
	Super::BeginPlay();
	GenerateVoxelLayers();
	CreateTexture();
	GenerateMesh();
	PaintOnTexture();
	UpdateTexture();
}

void AChunkActor::GenerateVoxelLayers()
{
	FIntVector deltas[] = {FIntVector(1,0,0), FIntVector(-1,0,0), FIntVector(0,1,0), FIntVector(0,-1,0), FIntVector(0,0,1), FIntVector(0,0,-1)};
	auto almost = DIMENSION - 1;
	for (auto x = 0; x < DIMENSION; x++)
	{
		for (auto y = 0; y < DIMENSION; y++)
		{
			for (auto z = 0; z < DIMENSION; z++) {
				auto current = x + y * DIMENSION + z * DIMENSION * DIMENSION;
				auto voxel = voxelArray[current];
				if (voxel == 0) continue;

				auto zA = current - DIMENSION * DIMENSION;
				auto zB = current + DIMENSION * DIMENSION;
				auto yA = current - DIMENSION;
				auto yB = current + DIMENSION;
				auto xA = current - 1;
				auto xB = current + 1;

				if (x == 0 || voxelArray[xA] == 0) layers[x] = 1;
				if (x == almost || voxelArray[xB] == 0) layers[x + 1] = 1;
				if (y == 0 || voxelArray[yA] == 0) layers[y + LAYERS] = 1;
				if (y == almost || voxelArray[yB] == 0) layers[y + LAYERS + 1] = 1;
				if (z == 0 || voxelArray[zA] == 0) layers[z + LAYERS * 2] = 1;
				if (z == almost || voxelArray[zB] == 0) layers[z + LAYERS * 2 + 1] = 1;
			}
		}
	}

	auto counter = 0;
	for (auto i = 0; i < LAYERS * 3; i++)
	{
		if (layers[i] != 0)
		{
			counter++;
		}
	}
	activeLayerCount = counter;
	//UE_LOG(LogTemp, Warning, TEXT("layers %d    %d"), LAYERS * 3, activeLayerCount);
}

void AChunkActor::CreateTexture()
{
	textureSizeX = TILE_DIMENSION * 4;
	textureSizeY = (activeLayerCount / 4 + 1) * TILE_DIMENSION;
	//textureSizeY = FMath::Pow(2, 1 + FMath::Log2((activeLayerCount / 4 + 1) * TILE_DIMENSION));
	pixelCount = textureSizeY * textureSizeX;
	bufferSize = pixelCount * BYTES_PER_PIXEL;
	bufferPitch = textureSizeX * BYTES_PER_PIXEL;

	uvSize = FVector2D(static_cast<float>(DIMENSION) / textureSizeX, static_cast<float>(DIMENSION) / textureSizeY);

	pixelData = new uint8[bufferSize]; // caution: mem-leak danger
	materialInstanceDynamic = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);

	// create dynamic texture
	texture = UTexture2D::CreateTransient(textureSizeX, textureSizeY);
	texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	texture->Filter = TextureFilter::TF_Nearest;
	texture->SRGB = 0;
	texture->UpdateResource();

	materialInstanceDynamic->SetTextureParameterValue(TEXTURE_PARAM_NAME, texture);
	updateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, textureSizeX, textureSizeY);
}

void AChunkActor::createPoligon(FVector start, FVector planeSize, FVector2D uvStart, FVector2D uvSize, FVector normal, FRuntimeMeshTangent tangent)
{
	FVector2D uv[] = { FVector2D(uvStart.X, uvStart.Y + uvSize.Y), uvStart + uvSize, FVector2D(uvStart.X + uvSize.X, uvStart.Y), uvStart };
	auto deltaA = FVector(planeSize);
	auto deltaB = FVector(planeSize);

	if (planeSize.X == 0)
	{
		deltaA.Y = 0;
		deltaB.Z = 0;
	}
	else if (planeSize.Y == 0)
	{
		deltaA.X = 0;
		deltaB.Z = 0;
	}
	else if (planeSize.Z == 0)
	{
		deltaA.X = 0;
		deltaB.Y = 0;
	}

	Vertices.Add(FRuntimeMeshVertexSimple(start + deltaA, normal, tangent, FColor::White, uv[0]));
	Vertices.Add(FRuntimeMeshVertexSimple(start + planeSize, normal, tangent, FColor::White, uv[1]));
	Vertices.Add(FRuntimeMeshVertexSimple(start + deltaB, normal, tangent, FColor::White, uv[2]));
	Vertices.Add(FRuntimeMeshVertexSimple(start, normal, tangent, FColor::White, uv[3]));

	auto count = Vertices.Num();
	Triangles.Add(count - 4);
	Triangles.Add(count - 3);
	Triangles.Add(count - 2);
	Triangles.Add(count - 4);
	Triangles.Add(count - 2);
	Triangles.Add(count - 1);
}

void AChunkActor::GenerateMesh()
{
	// Oversized bounding box so we don't have to update it
	BoundingBox = FBox(FVector(0, 0, 0), FVector(SIZE, SIZE, SIZE));

	TArray<TPair<FVector2D, FVector2D>> possibleTiles;

	auto xTiles = textureSizeX / TILE_DIMENSION;
	auto yTiles = textureSizeY / TILE_DIMENSION;

	for (auto i = 0; i < xTiles; i++)
	{
		for (auto j = 0; j < yTiles; j++)
		{
			auto pixelTile = FVector2D(i * TILE_DIMENSION, j * TILE_DIMENSION);
			auto pixelTileUV = FVector2D(pixelTile.X / textureSizeX, pixelTile.Y / textureSizeY);
			possibleTiles.Add(TPair<FVector2D, FVector2D>(pixelTile, pixelTileUV));
		}
	}

	for (auto index = 0; index < LAYERS * 3; index++)
	{
		pixelTiles.Add(FVector2D(0, 0));

		if (layers[index] == 0) continue;

		auto tile = possibleTiles.Pop();
		pixelTiles[index] = tile.Key;

		auto delta = index % LAYERS;
		if (index < LAYERS)
		{
			createPoligon(FVector(delta * SCALE, 0, 0), FVector(0, SIZE, SIZE), tile.Value, uvSize, FVector(1, 0, 0), FRuntimeMeshTangent(0, 0, -1));
		}
		else if (index < LAYERS * 2)
		{
			createPoligon(FVector(0, delta * SCALE, 0), FVector(SIZE, 0, SIZE), tile.Value, uvSize, FVector(0, 1, 0), FRuntimeMeshTangent(-1, 0, 0));
		}
		else if (index < LAYERS * 3)
		{
			createPoligon(FVector(0, 0, delta * SCALE), FVector(SIZE, SIZE, 0), tile.Value, uvSize, FVector(0, 0, 1), FRuntimeMeshTangent(0, -1, 0));
		}
	}

	//createPoligon(FVector(0, 0, LAYERS * SCALE), FVector(SIZE, SIZE, 0), FVector2D(0,0), FVector2D(1, 1), FVector(0, 0, 1), FRuntimeMeshTangent(0, -1, 0));
	if (Vertices.Num())
		MeshComponent->CreateMeshSection(0, Vertices, Triangles, BoundingBox, false, EUpdateFrequency::Infrequent);
}

void AChunkActor::PaintPixel(FVector2D pixel, FVector4 color) const
{
	int i = (pixel.X + pixel.Y * textureSizeX) * 4;
	auto iBlue = i;
	auto iGreen = i + 1;
	auto iRed = i + 2;
	auto iAlpha = i + 3;

	pixelData[iBlue] = color[0];
	pixelData[iGreen] = color[1];
	pixelData[iRed] = color[2];
	pixelData[iAlpha] = color[3];
}

void AChunkActor::DrawPosition(FIntVector pos, FVector4 color)
{
	auto x = pos.X, dx = x + 1;
	auto y = pos.Y + LAYERS, dy = y + 1;
	auto z = pos.Z + LAYERS * 2, dz = z + 1;

	if (layers[x] != 0) PaintPixel(pixelTiles[x] + FVector2D(pos.Y, pos.Z), color);
	if (layers[dx] != 0) PaintPixel(pixelTiles[dx] + FVector2D(pos.Y, pos.Z), color);
	if (layers[y] != 0) PaintPixel(pixelTiles[y] + FVector2D(pos.X, pos.Z), color);
	if (layers[dy] != 0) PaintPixel(pixelTiles[dy] + FVector2D(pos.X, pos.Z), color);
	if (layers[z] != 0) PaintPixel(pixelTiles[z] + FVector2D(pos.X, pos.Y), color);
	if (layers[dz] != 0) PaintPixel(pixelTiles[dz] + FVector2D(pos.X, pos.Y), color);
}

void AChunkActor::PaintOnTexture()
{
	for (auto i = 0; i < pixelCount; i++) {
		pixelData[i * 4 + 3] = 0;
	}

	FIntVector pos;
	auto almost = DIMENSION - 1;
	auto color = FVector4(0, 0, 0, 255);

	for (auto x = 0; x < DIMENSION; x++)
	{
		for (auto y = 0; y < DIMENSION; y++)
		{
			for (auto z = 0; z < DIMENSION; z++) {
				auto current = x + y * DIMENSION + z * DIMENSION * DIMENSION;
				auto voxel = voxelArray[current];
				if (voxel == 0) continue;

				auto zA = current - DIMENSION * DIMENSION;
				auto zB = current + DIMENSION * DIMENSION;
				auto yA = current - DIMENSION;
				auto yB = current + DIMENSION;
				auto xA = current - 1;
				auto xB = current + 1;

				if (x % almost == 0 || y % almost == 0 || z % almost == 0 ||
					voxelArray[xA] == 0 ||
					voxelArray[xB] == 0 ||
					voxelArray[yA] == 0 ||
					voxelArray[yB] == 0 ||
					voxelArray[zA] == 0 ||
					voxelArray[zB] == 0)
				{
					pos.X = x;
					pos.Y = y;
					pos.Z = z;
					if (voxel == 1)
					{
						color = FVector4(FMath::FRandRange(1, 4) * 4, FMath::FRandRange(40, 68), FMath::FRandRange(0, 1) * 2, 255);
					}
					else if (voxel == 2)
					{
						color = FVector4(FMath::FRandRange(0, 2) * 8, FMath::FRandRange(35, 45), FMath::FRandRange(60, 80), 255);
					}

					DrawPosition(pos, color);
				}
			}
		}
	}
}

void AChunkActor::UpdateTexture() const
{
	texture->UpdateTextureRegions(0, 1, updateTextureRegion, bufferPitch, BYTES_PER_PIXEL, pixelData);
	materialInstanceDynamic->SetTextureParameterValue(TEXTURE_PARAM_NAME, texture);
}
