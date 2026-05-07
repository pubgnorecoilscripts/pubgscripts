#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MallCollapseTypes.h"
#include "MCGameMode.generated.h"

class AMCGameState;

UCLASS()
class MALLCOLLAPSE_API AMCGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMCGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Match Flow", meta = (ClampMin = "0.0"))
	float EntryDurationSeconds = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Match Flow", meta = (ClampMin = "0.0"))
	float InstabilityDurationSeconds = 420.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Match Flow", meta = (ClampMin = "0.0"))
	float CollapseCascadeDurationSeconds = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Match Flow", meta = (ClampMin = "0.0"))
	float FinalExtractionDurationSeconds = 300.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Match Flow")
	float MatchElapsedSeconds = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Match Flow")
	float PhaseElapsedSeconds = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Match Flow")
	EMCMatchPhase CurrentPhase = EMCMatchPhase::Entry;

	void AdvanceMatchClock(float DeltaSeconds);
	void ChangePhase(EMCMatchPhase NewPhase);
	float GetCurrentPhaseDuration() const;
	AMCGameState* GetMallGameState() const;
};
