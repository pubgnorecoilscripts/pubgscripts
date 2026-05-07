#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MCInteractable.generated.h"

class APawn;

UINTERFACE(BlueprintType)
class MALLCOLLAPSE_API UMCInteractable : public UInterface
{
	GENERATED_BODY()
};

class MALLCOLLAPSE_API IMCInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool CanInteract(APawn* InteractingPawn) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(APawn* InteractingPawn);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionText() const;
};
