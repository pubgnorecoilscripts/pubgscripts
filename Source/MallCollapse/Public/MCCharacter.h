#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MCCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class UMCCarryComponent;
class UMCInteractionComponent;
class UMCPanicComponent;
class USceneComponent;
struct FInputActionValue;

UCLASS()
class MALLCOLLAPSE_API AMCCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMCCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure, Category = "Mall Collapse")
	UMCCarryComponent* GetCarryComponent() const { return CarryComponent; }

	UFUNCTION(BlueprintPure, Category = "Mall Collapse")
	UMCPanicComponent* GetPanicComponent() const { return PanicComponent; }

	UFUNCTION(BlueprintPure, Category = "Mall Collapse")
	UMCInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }

	UFUNCTION(BlueprintPure, Category = "Mall Collapse")
	USceneComponent* GetCarryAttachPoint() const { return CarryAttachPoint; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> CarryAttachPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMCCarryComponent> CarryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMCPanicComponent> PanicComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMCInteractionComponent> InteractionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float BaseWalkSpeed = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultInputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	int32 DefaultInputMappingPriority = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DropLootAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> PingAction;

	void ApplyMallMovementModifiers();
	void ApplyMovementVector(const FVector2D& MovementVector);
	void HandleMoveInput(const FInputActionValue& Value);
	void HandleLookInput(const FInputActionValue& Value);
	void HandleInteractInput();
	void HandleDropLootInput();
	void HandlePingInput();
	void HandleMoveForward(float Value);
	void HandleMoveRight(float Value);
	void HandleTurn(float Value);
	void HandleLookUp(float Value);
};
