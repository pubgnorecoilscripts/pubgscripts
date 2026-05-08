#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MCInteractable.h"
#include "MallCollapseTypes.h"
#include "MCSabotageDevice.generated.h"

class AController;
class AMCExtractionZone;
class AMCGameState;
class AMCHazardVolume;
class AMCMallModuleStateActor;
class UStaticMeshComponent;

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

	UFUNCTION(BlueprintCallable, Category = "Sabotage")
	void ConfigurePrototypeSabotage(EMCSabotageType NewSabotageType, AMCExtractionZone* ExtractionZone, AMCHazardVolume* HazardVolume, AMCMallModuleStateActor* MallModule);

	UPROPERTY(BlueprintAssignable, Category = "Sabotage")
	FMCSabotageActivatedSignature OnSabotageActivated;

	virtual bool CanInteract_Implementation(APawn* InteractingPawn) const override;
	virtual void Interact_Implementation(APawn* InteractingPawn) override;
	virtual FText GetInteractionText_Implementation() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DebugMeshComponent;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sabotage|Effects")
	TArray<TObjectPtr<AMCExtractionZone>> LinkedExtractionZones;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sabotage|Effects")
	TArray<TObjectPtr<AMCHazardVolume>> LinkedHazardVolumes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sabotage|Effects")
	TArray<TObjectPtr<AMCMallModuleStateActor>> LinkedMallModules;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sabotage|Effects", meta = (ClampMin = "0.0"))
	float ModuleStressAmount = 18.0f;

	UFUNCTION()
	void OnRep_SabotageState();

	UFUNCTION(BlueprintImplementableEvent, Category = "Sabotage")
	void HandleSabotageActivated(AController* InstigatorController);

	UFUNCTION(BlueprintImplementableEvent, Category = "Sabotage")
	void HandleSabotageReset();

	void ScheduleResetIfNeeded();
	void ApplySabotageEffects(AController* InstigatorController);
	void ApplyExtractionLockdown();
	void ApplyBreakerBlackout();
	void ApplyEscalatorShutdown();
	void ApplySprinklerFlooding();
	void ApplyFalseEmergencyAlarm();
	AMCGameState* GetMallGameState() const;
};
