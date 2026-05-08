#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MCGreyboxArena.generated.h"

class UInstancedStaticMeshComponent;

UCLASS()
class MALLCOLLAPSE_API AMCGreyboxArena : public AActor
{
	GENERATED_BODY()

public:
	AMCGreyboxArena();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Greybox")
	TObjectPtr<UInstancedStaticMeshComponent> FloorPieces;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Greybox")
	TObjectPtr<UInstancedStaticMeshComponent> WallPieces;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Greybox")
	TObjectPtr<UInstancedStaticMeshComponent> StorePieces;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Greybox")
	TObjectPtr<UInstancedStaticMeshComponent> RoutePieces;

	void AddBox(UInstancedStaticMeshComponent* Component, const FVector& Location, const FVector& Scale);
};
