#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MallCollapseTypes.h"
#include "MCMatchDirector.generated.h"

class AMCExtractionZone;
class AMCGameState;
class AMCHazardVolume;
class AMCMallModuleStateActor;

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
	void RegisterHazardVolume(AMCHazardVolume* HazardVolume);

	UFUNCTION(BlueprintCallable, Category = "Match Director")
	void ApplyPhasePlan(EMCMatchPhase MatchPhase);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director")
	TArray<TObjectPtr<AMCExtractionZone>> ExtractionZones;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director")
	TArray<TObjectPtr<AMCHazardVolume>> HazardEventTargets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director")
	TArray<TObjectPtr<AMCMallModuleStateActor>> MallModuleEventTargets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director")
	bool bAutoFindExtractionZones = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director")
	bool bAutoFindHazardVolumes = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director", meta = (ClampMin = "1"))
	int32 MinFinalAvailableExits = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director", meta = (ClampMin = "0"))
	int32 RandomSeed = 1337;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director")
	bool bEntryStartsWithUnknownBackupExits = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director|Events", meta = (ClampMin = "1.0"))
	float AnnouncementIntervalSeconds = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director|Events", meta = (ClampMin = "1.0"))
	float ExtractionPanicEventIntervalSeconds = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director|Events", meta = (ClampMin = "1.0"))
	float HazardEventIntervalSeconds = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director|Events", meta = (ClampMin = "1.0"))
	float AtmosphereCueIntervalSeconds = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director|Final Extraction", meta = (ClampMin = "1.0"))
	float FinalExtractionNarrowIntervalSeconds = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director|Announcements")
	TArray<FString> EmergencyAnnouncements;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director|Announcements")
	TArray<FString> FakeEvacuationAnnouncements;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director|Announcements")
	TArray<FString> SecurityAnnouncements;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director|Announcements")
	TArray<FString> PromotionAnnouncements;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Director|Announcements")
	TArray<FString> MalfunctionAnnouncements;

	UPROPERTY(BlueprintReadOnly, Category = "Match Director")
	EMCMatchPhase LastAppliedPhase = EMCMatchPhase::Entry;

	UPROPERTY(BlueprintReadOnly, Category = "Match Director")
	int32 FinalExtractionNarrowStep = 0;

	UFUNCTION(BlueprintImplementableEvent, Category = "Match Director")
	void HandlePhasePlanApplied(EMCMatchPhase MatchPhase);

	AMCGameState* GetMallGameState() const;
	void AutoFindExtractionZones();
	void AutoFindHazardVolumes();
	void ApplyEntryPlan();
	void ApplyInstabilityPlan();
	void ApplyCollapseCascadePlan();
	void ApplyFinalExtractionPlan();
	void ApplyMatchEndedPlan();
	void ShuffleExtractionZones(TArray<AMCExtractionZone*>& ZonesToShuffle) const;
	void InitializeDefaultAnnouncements();
	void AdvanceProceduralEvents(float DeltaSeconds);
	void TriggerProceduralAnnouncement();
	void TriggerExtractionPanicEvent();
	void TriggerDynamicHazardEvent();
	void TriggerAtmosphereCue();
	void IntensifyFinalExtraction(float DeltaSeconds);
	float GetPhaseIntensity() const;
	FString PickAnnouncement(const TArray<FString>& Pool, const FString& Fallback) const;

	float AnnouncementTimerSeconds = 0.0f;
	float ExtractionPanicTimerSeconds = 0.0f;
	float HazardEventTimerSeconds = 0.0f;
	float AtmosphereCueTimerSeconds = 0.0f;
	float FinalExtractionNarrowTimerSeconds = 0.0f;
};
