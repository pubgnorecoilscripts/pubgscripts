#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MCInteractionComponent.generated.h"

UCLASS(ClassGroup = (MallCollapse), meta = (BlueprintSpawnableComponent))
class MALLCOLLAPSE_API UMCInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMCInteractionComponent();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void InteractWithActor(AActor* TargetActor);

	UFUNCTION(Server, Reliable, Category = "Interaction")
	void ServerInteractWithActor(AActor* TargetActor);

	UFUNCTION(BlueprintPure, Category = "Interaction")
	bool IsActorInteractable(AActor* TargetActor) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float MaxInteractionDistance = 250.0f;

	bool IsWithinInteractionRange(AActor* TargetActor) const;
};
