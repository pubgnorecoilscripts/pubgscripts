#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MallCollapseTypes.h"
#include "MCGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMCMatchPhaseChangedSignature, EMCMatchPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMCMallAnnouncementSignature, FMCMallAnnouncement, Announcement);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMCAtmosphereCueSignature, FMCAtmosphereCue, AtmosphereCue);

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

	UFUNCTION(BlueprintCallable, Category = "Announcement")
	void BroadcastMallAnnouncement(EMCAnnouncementType AnnouncementType, const FString& Message, bool bFake, float Intensity);

	UFUNCTION(BlueprintCallable, Category = "Atmosphere")
	void BroadcastAtmosphereCue(EMCAtmosphereCueType CueType, float Intensity, FVector Location, float Radius);

	UFUNCTION(BlueprintPure, Category = "Match")
	EMCMatchPhase GetMatchPhase() const { return MatchPhase; }

	UFUNCTION(BlueprintPure, Category = "Match")
	float GetMatchElapsedSeconds() const { return MatchElapsedSeconds; }

	UFUNCTION(BlueprintPure, Category = "Match")
	float GetPhaseElapsedSeconds() const { return PhaseElapsedSeconds; }

	UFUNCTION(BlueprintPure, Category = "Extraction")
	int32 GetTotalExtractedValue() const { return TotalExtractedValue; }

	UFUNCTION(BlueprintPure, Category = "Extraction")
	TArray<FMCPlayerExtractionResult> GetExtractionResults() const { return ExtractionResults; }

	UFUNCTION(BlueprintPure, Category = "Announcement")
	FMCMallAnnouncement GetLastAnnouncement() const { return LastAnnouncement; }

	UFUNCTION(BlueprintPure, Category = "Atmosphere")
	FMCAtmosphereCue GetLastAtmosphereCue() const { return LastAtmosphereCue; }

	UPROPERTY(BlueprintAssignable, Category = "Match")
	FMCMatchPhaseChangedSignature OnMatchPhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "Announcement")
	FMCMallAnnouncementSignature OnMallAnnouncement;

	UPROPERTY(BlueprintAssignable, Category = "Atmosphere")
	FMCAtmosphereCueSignature OnAtmosphereCue;

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

	UPROPERTY(ReplicatedUsing = OnRep_LastAnnouncement, BlueprintReadOnly, Category = "Announcement")
	FMCMallAnnouncement LastAnnouncement;

	UPROPERTY(ReplicatedUsing = OnRep_LastAtmosphereCue, BlueprintReadOnly, Category = "Atmosphere")
	FMCAtmosphereCue LastAtmosphereCue;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Announcement")
	int32 AnnouncementSequence = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Atmosphere")
	int32 AtmosphereSequence = 0;

	UFUNCTION()
	void OnRep_MatchPhase();

	UFUNCTION()
	void OnRep_LastAnnouncement();

	UFUNCTION()
	void OnRep_LastAtmosphereCue();
};
