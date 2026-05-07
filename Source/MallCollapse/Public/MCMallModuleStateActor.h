#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MallCollapseTypes.h"
#include "MCMallModuleStateActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMCIntegrityStateChangedSignature, EMCModuleIntegrityState, NewIntegrityState);

UCLASS(Blueprintable)
class MALLCOLLAPSE_API AMCMallModuleStateActor : public AActor
{
	GENERATED_BODY()

public:
	AMCMallModuleStateActor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Mall Module")
	void ApplyFireDamage(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Mall Module")
	void ApplyWaterDamage(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Mall Module")
	void ApplyOverloadStress(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Mall Module")
	void ApplyElectricalInstability(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Mall Module")
	void SetIntegrityState(EMCModuleIntegrityState NewState);

	UFUNCTION(BlueprintPure, Category = "Mall Module")
	EMCModuleIntegrityState GetIntegrityState() const { return IntegrityState; }

	UPROPERTY(BlueprintAssignable, Category = "Mall Module")
	FMCIntegrityStateChangedSignature OnIntegrityStateChanged;

protected:
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_IntegrityState, BlueprintReadOnly, Category = "Mall Module")
	EMCModuleIntegrityState IntegrityState = EMCModuleIntegrityState::Intact;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Mall Module", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float FireDamage = 0.0f;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Mall Module", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float WaterDamage = 0.0f;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Mall Module", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float OverloadStress = 0.0f;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Mall Module", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float ElectricalInstability = 0.0f;

	UFUNCTION()
	void OnRep_IntegrityState();

	UFUNCTION(BlueprintImplementableEvent, Category = "Mall Module")
	void HandleIntegrityStateChanged();

	void RecalculateIntegrityState();
	float GetCombinedDamageScore() const;
};
