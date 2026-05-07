#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MCInteractable.h"
#include "MallCollapseTypes.h"
#include "MCLootItem.generated.h"

class UStaticMeshComponent;

UCLASS(Blueprintable)
class MALLCOLLAPSE_API AMCLootItem : public AActor, public IMCInteractable
{
	GENERATED_BODY()

public:
	AMCLootItem();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Loot")
	bool CanBePickedUp() const;

	UFUNCTION(BlueprintCallable, Category = "Loot")
	void MarkCarried(AActor* NewCarrier);

	UFUNCTION(BlueprintCallable, Category = "Loot")
	void MarkDropped(const FVector& DropLocation);

	UFUNCTION(BlueprintCallable, Category = "Loot")
	void MarkExtracted();

	UFUNCTION(BlueprintPure, Category = "Loot")
	FMCLootDescriptor GetLootDescriptor() const;

	UFUNCTION(BlueprintPure, Category = "Loot")
	bool IsCarried() const { return bCarried; }

	UFUNCTION(BlueprintPure, Category = "Loot")
	bool IsExtracted() const { return bExtracted; }

	UFUNCTION(BlueprintPure, Category = "Loot")
	AActor* GetCarrier() const { return Carrier.Get(); }

	virtual bool CanInteract_Implementation(APawn* InteractingPawn) const override;
	virtual void Interact_Implementation(APawn* InteractingPawn) override;
	virtual FText GetInteractionText_Implementation() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Loot")
	FMCLootDescriptor LootDescriptor;

	UPROPERTY(ReplicatedUsing = OnRep_Carried, BlueprintReadOnly, Category = "Loot")
	bool bCarried = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Loot")
	bool bExtracted = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Loot")
	TObjectPtr<AActor> Carrier;

	UFUNCTION()
	void OnRep_Carried();

	void ApplyCarryPresentation();
};
