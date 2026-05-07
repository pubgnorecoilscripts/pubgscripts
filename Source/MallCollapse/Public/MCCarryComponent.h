#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MallCollapseTypes.h"
#include "MCCarryComponent.generated.h"

class AMCLootItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMCCarryStateChangedSignature, EMCCarryState, NewCarryState);

UCLASS(ClassGroup = (MallCollapse), meta = (BlueprintSpawnableComponent))
class MALLCOLLAPSE_API UMCCarryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMCCarryComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Carry")
	bool AddLoot(AMCLootItem* LootItem);

	UFUNCTION(BlueprintCallable, Category = "Carry")
	bool DropLoot(AMCLootItem* LootItem);

	UFUNCTION(BlueprintCallable, Category = "Carry")
	void DropAllLoot();

	UFUNCTION(BlueprintCallable, Category = "Carry")
	int32 ExtractAllLoot();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Carry")
	void ServerDropAllLoot();

	UFUNCTION(BlueprintCallable, Category = "Carry")
	void SetDragging(bool bNewDragging);

	UFUNCTION(BlueprintPure, Category = "Carry")
	float GetMovementSpeedMultiplier() const;

	UFUNCTION(BlueprintPure, Category = "Carry")
	EMCCarryState GetCarryState() const { return CarryState; }

	UFUNCTION(BlueprintPure, Category = "Carry")
	int32 GetCurrentCarryValue() const { return CurrentCarryValue; }

	UFUNCTION(BlueprintPure, Category = "Carry")
	float GetCurrentCarryWeight() const { return CurrentCarryWeight; }

	UFUNCTION(BlueprintPure, Category = "Carry|Risk")
	float GetCurrentVisionObstruction() const { return CurrentVisionObstruction; }

	UFUNCTION(BlueprintPure, Category = "Carry|Risk")
	float GetCurrentCarriedNoiseRadius() const { return CurrentCarriedNoiseRadius; }

	UFUNCTION(BlueprintPure, Category = "Carry|Risk")
	bool IsCarryingTwoHandedLoot() const { return bCarryingTwoHandedLoot; }

	UPROPERTY(BlueprintAssignable, Category = "Carry")
	FMCCarryStateChangedSignature OnCarryStateChanged;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Carry")
	float EncumberedWeightThreshold = 18.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Carry")
	float OverloadedWeightThreshold = 38.0f;

	UPROPERTY(ReplicatedUsing = OnRep_CarryState, BlueprintReadOnly, Category = "Carry")
	EMCCarryState CarryState = EMCCarryState::Light;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Carry")
	float CurrentCarryWeight = 0.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Carry")
	int32 CurrentCarryValue = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Carry")
	bool bDragging = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Carry|Risk")
	float CurrentVisionObstruction = 0.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Carry|Risk")
	float CurrentCarriedNoiseRadius = 0.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Carry|Risk")
	bool bCarryingTwoHandedLoot = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Carry|Risk")
	float CurrentRiskPanicPerSecond = 0.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Carry|Risk")
	float CurrentLootMovementPenaltyMultiplier = 1.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Carry")
	TArray<TObjectPtr<AMCLootItem>> CarriedItems;

	UFUNCTION()
	void OnRep_CarryState();

	void RecalculateCarryState();
	FVector GetDropLocation(int32 DropIndex) const;
};
