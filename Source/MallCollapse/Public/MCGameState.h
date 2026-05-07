#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MallCollapseTypes.h"
#include "MCGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMCMatchPhaseChangedSignature, EMCMatchPhase, NewPhase);

UCLASS()
class MALLCOLLAPSE_API AMCGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMCGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Match")
	void SetMatchPhase(EMCMatchPhase NewPhase);

	UFUNCTION(BlueprintCallable, Category = "Match")
	void SetMatchClock(float NewMatchElapsedSeconds, float NewPhaseElapsedSeconds);

	UFUNCTION(BlueprintCallable, Category = "Extraction")
	void AddExtractedValue(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Extraction")
	void RecordPlayerExtraction(AController* ExtractingController, int32 Value);

	UFUNCTION(BlueprintPure, Category = "Match")
	EMCMatchPhase GetMatchPhase() const { return MatchPhase; }

	UFUNCTION(BlueprintPure, Category = "Match")
	float GetMatchElapsedSeconds() const { return MatchElapsedSeconds; }

	UFUNCTION(BlueprintPure, Category = "Match")
	float GetPhaseElapsedSeconds() const { return PhaseElapsedSeconds; }

	UFUNCTION(BlueprintPure, Category = "Extraction")
	int32 GetTotalExtractedValue() const { return TotalExtractedValue; }

	UFUNCTION(BlueprintPure, Category = "Extraction")
	const TArray<FMCPlayerExtractionResult>& GetExtractionResults() const { return ExtractionResults; }

	UPROPERTY(BlueprintAssignable, Category = "Match")
	FMCMatchPhaseChangedSignature OnMatchPhaseChanged;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_MatchPhase, BlueprintReadOnly, Category = "Match")
	EMCMatchPhase MatchPhase = EMCMatchPhase::Entry;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Match")
	float MatchElapsedSeconds = 0.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Match")
	float PhaseElapsedSeconds = 0.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Extraction")
	int32 TotalExtractedValue = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Extraction")
	TArray<FMCPlayerExtractionResult> ExtractionResults;

	UFUNCTION()
	void OnRep_MatchPhase();
};
