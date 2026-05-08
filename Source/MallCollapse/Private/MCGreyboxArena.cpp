#include "MCGreyboxArena.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

AMCGreyboxArena::AMCGreyboxArena()
{
	bReplicates = true;

	FloorPieces = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FloorPieces"));
	SetRootComponent(FloorPieces);

	WallPieces = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("WallPieces"));
	WallPieces->SetupAttachment(FloorPieces);

	StorePieces = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("StorePieces"));
	StorePieces->SetupAttachment(FloorPieces);

	RoutePieces = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("RoutePieces"));
	RoutePieces->SetupAttachment(FloorPieces);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		FloorPieces->SetStaticMesh(CubeMesh.Object);
		WallPieces->SetStaticMesh(CubeMesh.Object);
		StorePieces->SetStaticMesh(CubeMesh.Object);
		RoutePieces->SetStaticMesh(CubeMesh.Object);
	}

	FloorPieces->SetCollisionProfileName(TEXT("BlockAll"));
	WallPieces->SetCollisionProfileName(TEXT("BlockAll"));
	StorePieces->SetCollisionProfileName(TEXT("BlockAll"));
	RoutePieces->SetCollisionProfileName(TEXT("BlockAll"));

	// Tiny ugly mall: central atrium, side store, maintenance hallway, alternate route.
	AddBox(FloorPieces, FVector(0.0f, 0.0f, -55.0f), FVector(12.0f, 9.0f, 0.1f));
	AddBox(FloorPieces, FVector(-900.0f, 0.0f, -55.0f), FVector(6.0f, 4.0f, 0.1f));
	AddBox(FloorPieces, FVector(900.0f, 0.0f, -55.0f), FVector(6.0f, 3.0f, 0.1f));
	AddBox(FloorPieces, FVector(0.0f, 800.0f, -55.0f), FVector(10.0f, 2.0f, 0.1f));

	AddBox(WallPieces, FVector(0.0f, -500.0f, 150.0f), FVector(12.0f, 0.25f, 3.0f));
	AddBox(WallPieces, FVector(0.0f, 1025.0f, 150.0f), FVector(12.0f, 0.25f, 3.0f));
	AddBox(WallPieces, FVector(-1250.0f, 250.0f, 150.0f), FVector(0.25f, 7.0f, 3.0f));
	AddBox(WallPieces, FVector(1250.0f, 250.0f, 150.0f), FVector(0.25f, 7.0f, 3.0f));
	AddBox(WallPieces, FVector(-450.0f, 500.0f, 120.0f), FVector(0.2f, 1.0f, 2.4f));
	AddBox(WallPieces, FVector(450.0f, 500.0f, 120.0f), FVector(0.2f, 1.0f, 2.4f));

	AddBox(StorePieces, FVector(-900.0f, -250.0f, 40.0f), FVector(1.4f, 0.25f, 0.9f));
	AddBox(StorePieces, FVector(-900.0f, 0.0f, 40.0f), FVector(1.4f, 0.25f, 0.9f));
	AddBox(StorePieces, FVector(-900.0f, 250.0f, 40.0f), FVector(1.4f, 0.25f, 0.9f));
	AddBox(RoutePieces, FVector(450.0f, 800.0f, 30.0f), FVector(0.25f, 1.4f, 0.6f));
}

void AMCGreyboxArena::AddBox(UInstancedStaticMeshComponent* Component, const FVector& Location, const FVector& Scale)
{
	if (!Component)
	{
		return;
	}

	Component->AddInstance(FTransform(FRotator::ZeroRotator, Location, Scale));
}
