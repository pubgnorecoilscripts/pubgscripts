#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MCInteractable.h"
#include "MallCollapseTypes.h"
#include "MCSabotageDevice.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMCSabotageActivatedSignature, EMCSabotageType, SabotageType, AController*, InstigatorController);

UCLASS(Blueprintable)
class MALLCOLLAPSE_API AMCSabotageDevice : public AActor, public IMCInteractable
{
	GENERATED_BODY()

public:
	AMCSabotageDevice();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Sabotage")
	bool ActivateSabotage(AController* InstigatorController);

	UFUNCTION(BlueprintPure, Category = "Sabotage")
	bool CanActivateSabotage() const;

	UFUNCTION(BlueprintCallable, Category = "Sabotage")
	void ResetSabotage();

	UPROPERTY(BlueprintAssignable, Category = "Sabotage")
	FMCSabotageActivatedSignature OnSabotageActivated;

	virtual bool CanInteract_Implementation(APawn* InteractingPawn) const override;
	virtual void Interact_Implementation(APawn* InteractingPawn) override;
	virtual FText GetInteractionText_Implementation() const override;

protected:
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_SabotageState, BlueprintReadOnly, Category = "Sabotage")
	EMCSabotageType SabotageType = EMCSabotageType::LockdownShutter;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_SabotageState, BlueprintReadOnly, Category = "Sabotage")
	bool bActivated = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sabotage")
	bool bOneShot = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sabotage", meta = (ClampMin = "0.0"))
	float CooldownSeconds = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sabotage", meta = (ClampMin = "0.0"))
	float ActiveDurationSeconds = 12.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Sabotage")
	float LastActivationTimeSeconds = -1000.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Sabotage")
	TObjectPtr<AController> LastInstigatorController;

	UFUNCTION()
	void OnRep_SabotageState();

	UFUNCTION(BlueprintImplementableEvent, Category = "Sabotage")
	void HandleSabotageActivated(AController* InstigatorController);

	UFUNCTION(BlueprintImplementableEvent, Category = "Sabotage")
	void HandleSabotageReset();

	void ScheduleResetIfNeeded();
};
