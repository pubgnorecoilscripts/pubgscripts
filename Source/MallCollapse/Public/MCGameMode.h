#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MallCollapseTypes.h"
#include "MCGameMode.generated.h"

class AMCGameState;
class AMCGreyboxArena;
class AMCLootItem;
class AMCExtractionZone;
class AMCHazardVolume;
class AMCSabotageDevice;
class AMCMatchDirector;
class AMCMallModuleStateActor;

UCLASS()
class MALLCOLLAPSE_API AMCGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMCGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void RestartPlayer(AController* NewPlayer) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First Playable")
	bool bSpawnRuntimeGreybox = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First Playable")
	TSubclassOf<AMCGreyboxArena> GreyboxArenaClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First Playable")
	TSubclassOf<AMCLootItem> LootItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First Playable")
	TSubclassOf<AMCExtractionZone> ExtractionZoneClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First Playable")
	TSubclassOf<AMCHazardVolume> HazardVolumeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First Playable")
	TSubclassOf<AMCSabotageDevice> SabotageDeviceClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First Playable")
	TSubclassOf<AMCMatchDirector> MatchDirectorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First Playable")
	TSubclassOf<AMCMallModuleStateActor> MallModuleClass;

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
	void SpawnRuntimeGreybox();
	FTransform GetPrototypeSpawnTransform();

	int32 PrototypeSpawnIndex = 0;
	bool bRuntimeGreyboxSpawned = false;
};
