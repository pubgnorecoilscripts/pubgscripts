#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MallCollapseTypes.h"
#include "MCMatchDirector.generated.h"

class AMCExtractionZone;
class AMCGameState;

UCLASS(Blueprintable)
class MALLCOLLAPSE_API AMCMatchDirector : public AActor
{
	GENERATED_BODY()

public:
	AMCMatchDirector();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Match Director")
	void RegisterExtractionZone(AMCExtractionZone* ExtractionZone);

	UFUNCTION(BlueprintCallable, Category = "Match Director")
	void ApplyPhasePlan(EMCMatchPhase MatchPhase);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director")
	TArray<TObjectPtr<AMCExtractionZone>> ExtractionZones;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director")
	bool bAutoFindExtractionZones = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director", meta = (ClampMin = "1"))
	int32 MinFinalAvailableExits = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director", meta = (ClampMin = "0"))
	int32 RandomSeed = 1337;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director")
	bool bEntryStartsWithUnknownBackupExits = true;

	UPROPERTY(BlueprintReadOnly, Category = "Match Director")
	EMCMatchPhase LastAppliedPhase = EMCMatchPhase::Entry;

	UFUNCTION(BlueprintImplementableEvent, Category = "Match Director")
	void HandlePhasePlanApplied(EMCMatchPhase MatchPhase);

	AMCGameState* GetMallGameState() const;
	void AutoFindExtractionZones();
	void ApplyEntryPlan();
	void ApplyInstabilityPlan();
	void ApplyCollapseCascadePlan();
	void ApplyFinalExtractionPlan();
	void ApplyMatchEndedPlan();
	void ShuffleExtractionZones(TArray<AMCExtractionZone*>& ZonesToShuffle) const;
};
