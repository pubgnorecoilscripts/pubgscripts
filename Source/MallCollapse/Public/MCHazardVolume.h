#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MallCollapseTypes.h"
#include "MCHazardVolume.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class MALLCOLLAPSE_API AMCHazardVolume : public AActor
{
	GENERATED_BODY()

public:
	AMCHazardVolume();

	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Hazard")
	void SetHazardActive(bool bNewActive);

	UFUNCTION(BlueprintCallable, Category = "Hazard")
	void SetIntensity(float NewIntensity);

	UFUNCTION(BlueprintPure, Category = "Hazard")
	bool IsHazardActive() const { return bActive; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> HazardBounds;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_HazardState, BlueprintReadOnly, Category = "Hazard")
	EMCHazardType HazardType = EMCHazardType::Fire;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_HazardState, BlueprintReadOnly, Category = "Hazard")
	bool bActive = true;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Hazard", meta = (ClampMin = "0.0"))
	float Intensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hazard", meta = (ClampMin = "0.0"))
	float PanicPerSecond = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hazard", meta = (ClampMin = "0.0"))
	float DamagePerSecond = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hazard")
	bool bDamagePlayers = false;

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	UFUNCTION()
	void OnRep_HazardState();

	UFUNCTION(BlueprintImplementableEvent, Category = "Hazard")
	void HandleHazardStateChanged();

private:
	TSet<TWeakObjectPtr<AActor>> Occupants;
};
