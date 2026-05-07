#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MCCharacter.generated.h"

class UMCCarryComponent;
class UMCInteractionComponent;
class UMCPanicComponent;

UCLASS()
class MALLCOLLAPSE_API AMCCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMCCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, Category = "Mall Collapse")
	UMCCarryComponent* GetCarryComponent() const { return CarryComponent; }

	UFUNCTION(BlueprintPure, Category = "Mall Collapse")
	UMCPanicComponent* GetPanicComponent() const { return PanicComponent; }

	UFUNCTION(BlueprintPure, Category = "Mall Collapse")
	UMCInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMCCarryComponent> CarryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMCPanicComponent> PanicComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMCInteractionComponent> InteractionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float BaseWalkSpeed = 600.0f;

	void ApplyMallMovementModifiers();
};
