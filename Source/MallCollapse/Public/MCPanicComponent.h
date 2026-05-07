#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MallCollapseTypes.h"
#include "MCPanicComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMCPanicChangedSignature, float, Panic, EMCPanicTier, PanicTier);

UCLASS(ClassGroup = (MallCollapse), meta = (BlueprintSpawnableComponent))
class MALLCOLLAPSE_API UMCPanicComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMCPanicComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Panic")
	void AddPanic(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Panic")
	void RecoverPanic(float Amount);

	UFUNCTION(BlueprintPure, Category = "Panic")
	float GetPanic() const { return Panic; }

	UFUNCTION(BlueprintPure, Category = "Panic")
	EMCPanicTier GetPanicTier() const;

	UFUNCTION(BlueprintPure, Category = "Panic")
	float GetMovementStabilityMultiplier() const;

	UFUNCTION(BlueprintPure, Category = "Panic")
	float GetInteractionFumbleChance() const;

	UPROPERTY(BlueprintAssignable, Category = "Panic")
	FMCPanicChangedSignature OnPanicChanged;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Panic")
	float MaxPanic = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Panic")
	float PassiveRecoveryPerSecond = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Panic")
	bool bPassiveRecoveryEnabled = true;

	UPROPERTY(ReplicatedUsing = OnRep_Panic, BlueprintReadOnly, Category = "Panic")
	float Panic = 0.0f;

	UFUNCTION()
	void OnRep_Panic();

	void BroadcastPanicChanged();
};
