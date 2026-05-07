#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MCInteractionComponent.generated.h"

class AActor;

UCLASS(ClassGroup = (MallCollapse), meta = (BlueprintSpawnableComponent))
class MALLCOLLAPSE_API UMCInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMCInteractionComponent();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void InteractWithActor(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteractFromView();

	UFUNCTION(Server, Reliable, Category = "Interaction")
	void ServerInteractWithActor(AActor* TargetActor);

	UFUNCTION(Server, Reliable, Category = "Interaction")
	void ServerInteractFromView();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool FindInteractableFromView(FHitResult& OutHitResult) const;

	UFUNCTION(BlueprintPure, Category = "Interaction")
	bool IsActorInteractable(AActor* TargetActor) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float MaxInteractionDistance = 250.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float InteractionTraceRadius = 24.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> InteractionTraceChannel = ECC_Visibility;

	bool IsWithinInteractionRange(AActor* TargetActor) const;
	void ProcessInteraction(AActor* TargetActor);
	void ProcessInteractionFromView();
};
