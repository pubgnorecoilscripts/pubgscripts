#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MallCollapseTypes.h"
#include "TimerManager.h"
#include "MCExtractionZone.generated.h"

class UStaticMeshComponent;
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

	UFUNCTION(BlueprintCallable, Category = "Extraction")
	void TriggerExtractionPanicEvent(EMCExtractionPanicEventType PanicEventType, float DurationSeconds);

	UFUNCTION(BlueprintPure, Category = "Extraction")
	EMCExtractionState GetExtractionState() const { return ExtractionState; }

	UPROPERTY(BlueprintAssignable, Category = "Extraction")
	FMCExtractionSignature OnActorExtracted;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> ExtractionBounds;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DebugMeshComponent;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_ExtractionState, BlueprintReadOnly, Category = "Extraction")
	EMCExtractionState ExtractionState = EMCExtractionState::Available;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Extraction")
	bool bHideExtractedActors = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Extraction")
	bool bDisableExtractedActorCollision = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Extraction|Panic", meta = (ClampMin = "0.0"))
	float ExtractionDelaySeconds = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Extraction|Panic", meta = (ClampMin = "0.0"))
	float DefaultPanicEventDurationSeconds = 8.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Extraction|Panic")
	EMCExtractionPanicEventType LastPanicEventType = EMCExtractionPanicEventType::LastSecondCountdown;

	UPROPERTY(ReplicatedUsing = OnRep_LastPanicEvent, BlueprintReadOnly, Category = "Extraction|Panic")
	int32 PanicEventSequence = 0;

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	UFUNCTION()
	void OnRep_ExtractionState();

	UFUNCTION()
	void OnRep_LastPanicEvent();

	UFUNCTION(BlueprintImplementableEvent, Category = "Extraction")
	void HandleExtractionCountdownStarted(AActor* ActorToExtract, float CountdownSeconds);

	UFUNCTION(BlueprintImplementableEvent, Category = "Extraction")
	void HandleExtractionCountdownCancelled(AActor* ActorToExtract);

	UFUNCTION(BlueprintImplementableEvent, Category = "Extraction")
	void HandleExtractionPanicEvent(EMCExtractionPanicEventType PanicEventType, int32 SequenceId);

	UFUNCTION(BlueprintImplementableEvent, Category = "Extraction")
	void HandleExtractionStateChanged();

private:
	TSet<TWeakObjectPtr<AActor>> PendingExtractionActors;
	TMap<TWeakObjectPtr<AActor>, FTimerHandle> PendingExtractionTimers;
	FTimerHandle TemporaryStateTimerHandle;
	EMCExtractionState StateBeforeTemporaryPanicEvent = EMCExtractionState::Available;

	void BeginExtractionAttempt(AActor* ActorToExtract);
	void CancelExtractionAttempt(AActor* ActorToExtract);
	void CompleteDelayedExtraction(AActor* ActorToExtract);
	void RestoreTemporaryExtractionState();
};
