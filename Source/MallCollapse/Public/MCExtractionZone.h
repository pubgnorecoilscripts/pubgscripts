#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MallCollapseTypes.h"
#include "MCExtractionZone.generated.h"

class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMCExtractionSignature, AActor*, ExtractedActor, int32, ExtractedValue);

UCLASS(Blueprintable)
class MALLCOLLAPSE_API AMCExtractionZone : public AActor
{
	GENERATED_BODY()

public:
	AMCExtractionZone();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Extraction")
	bool TryExtractActor(AActor* ActorToExtract);

	UFUNCTION(BlueprintCallable, Category = "Extraction")
	void SetExtractionState(EMCExtractionState NewState);

	UFUNCTION(BlueprintPure, Category = "Extraction")
	EMCExtractionState GetExtractionState() const { return ExtractionState; }

	UPROPERTY(BlueprintAssignable, Category = "Extraction")
	FMCExtractionSignature OnActorExtracted;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> ExtractionBounds;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_ExtractionState, BlueprintReadOnly, Category = "Extraction")
	EMCExtractionState ExtractionState = EMCExtractionState::Available;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Extraction")
	bool bHideExtractedActors = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Extraction")
	bool bDisableExtractedActorCollision = true;

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRep_ExtractionState();

	UFUNCTION(BlueprintImplementableEvent, Category = "Extraction")
	void HandleExtractionStateChanged();
};
