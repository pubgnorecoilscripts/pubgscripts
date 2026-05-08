#include "MCGameMode.h"

#include "MCCharacter.h"
#include "MCDebugHUD.h"
#include "MCExtractionZone.h"
#include "MCGameState.h"
#include "MCGreyboxArena.h"
#include "MCHazardVolume.h"
#include "MCLootItem.h"
#include "MCMallModuleStateActor.h"
#include "MCMatchDirector.h"
#include "MCPlayerController.h"
#include "MCSabotageDevice.h"
#include "Engine/World.h"

AMCGameMode::AMCGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	GameStateClass = AMCGameState::StaticClass();
	DefaultPawnClass = AMCCharacter::StaticClass();
	PlayerControllerClass = AMCPlayerController::StaticClass();
	HUDClass = AMCDebugHUD::StaticClass();

	GreyboxArenaClass = AMCGreyboxArena::StaticClass();
	LootItemClass = AMCLootItem::StaticClass();
	ExtractionZoneClass = AMCExtractionZone::StaticClass();
	HazardVolumeClass = AMCHazardVolume::StaticClass();
	SabotageDeviceClass = AMCSabotageDevice::StaticClass();
	MatchDirectorClass = AMCMatchDirector::StaticClass();
	MallModuleClass = AMCMallModuleStateActor::StaticClass();

	EntryDurationSeconds = 60.0f;
	InstabilityDurationSeconds = 90.0f;
	CollapseCascadeDurationSeconds = 90.0f;
	FinalExtractionDurationSeconds = 60.0f;
}

void AMCGameMode::BeginPlay()
{
	Super::BeginPlay();

	SpawnRuntimeGreybox();
	ChangePhase(EMCMatchPhase::Entry);
}

void AMCGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AdvanceMatchClock(DeltaSeconds);
}

void AMCGameMode::RestartPlayer(AController* NewPlayer)
{
	if (!NewPlayer)
	{
		return;
	}

	RestartPlayerAtTransform(NewPlayer, GetPrototypeSpawnTransform());
}

void AMCGameMode::AdvanceMatchClock(float DeltaSeconds)
{
	if (CurrentPhase == EMCMatchPhase::MatchEnded)
	{
		return;
	}

	MatchElapsedSeconds += DeltaSeconds;
	PhaseElapsedSeconds += DeltaSeconds;

	const float PhaseDuration = GetCurrentPhaseDuration();
	if (PhaseDuration > 0.0f && PhaseElapsedSeconds >= PhaseDuration)
	{
		switch (CurrentPhase)
		{
		case EMCMatchPhase::Entry:
			ChangePhase(EMCMatchPhase::Instability);
			break;
		case EMCMatchPhase::Instability:
			ChangePhase(EMCMatchPhase::CollapseCascade);
			break;
		case EMCMatchPhase::CollapseCascade:
			ChangePhase(EMCMatchPhase::FinalExtraction);
			break;
		case EMCMatchPhase::FinalExtraction:
			ChangePhase(EMCMatchPhase::MatchEnded);
			break;
		default:
			break;
		}
	}

	if (AMCGameState* MallGameState = GetMallGameState())
	{
		MallGameState->SetMatchClock(MatchElapsedSeconds, PhaseElapsedSeconds);
	}
}

void AMCGameMode::ChangePhase(EMCMatchPhase NewPhase)
{
	CurrentPhase = NewPhase;
	PhaseElapsedSeconds = 0.0f;

	if (AMCGameState* MallGameState = GetMallGameState())
	{
		MallGameState->SetMatchPhase(NewPhase);
		MallGameState->SetMatchClock(MatchElapsedSeconds, PhaseElapsedSeconds);
	}
}

float AMCGameMode::GetCurrentPhaseDuration() const
{
	switch (CurrentPhase)
	{
	case EMCMatchPhase::Entry:
		return EntryDurationSeconds;
	case EMCMatchPhase::Instability:
		return InstabilityDurationSeconds;
	case EMCMatchPhase::CollapseCascade:
		return CollapseCascadeDurationSeconds;
	case EMCMatchPhase::FinalExtraction:
		return FinalExtractionDurationSeconds;
	default:
		return 0.0f;
	}
}

AMCGameState* AMCGameMode::GetMallGameState() const
{
	return Cast<AMCGameState>(GameState);
}

void AMCGameMode::SpawnRuntimeGreybox()
{
	if (!bSpawnRuntimeGreybox || bRuntimeGreyboxSpawned || !GetWorld())
	{
		return;
	}

	bRuntimeGreyboxSpawned = true;

	GetWorld()->SpawnActor<AMCGreyboxArena>(GreyboxArenaClass, FVector::ZeroVector, FRotator::ZeroRotator);

	AMCMallModuleStateActor* MallModule = GetWorld()->SpawnActor<AMCMallModuleStateActor>(MallModuleClass, FVector::ZeroVector, FRotator::ZeroRotator);

	AMCExtractionZone* MainExtraction = GetWorld()->SpawnActor<AMCExtractionZone>(ExtractionZoneClass, FVector(900.0f, -250.0f, 80.0f), FRotator::ZeroRotator);
	if (MainExtraction)
	{
		MainExtraction->SetActorScale3D(FVector(2.0f, 2.0f, 2.0f));
	}

	AMCExtractionZone* BackupExtraction = GetWorld()->SpawnActor<AMCExtractionZone>(ExtractionZoneClass, FVector(0.0f, 850.0f, 80.0f), FRotator::ZeroRotator);
	if (BackupExtraction)
	{
		BackupExtraction->SetActorScale3D(FVector(1.6f, 1.6f, 2.0f));
	}

	AMCHazardVolume* Hazard = GetWorld()->SpawnActor<AMCHazardVolume>(HazardVolumeClass, FVector(450.0f, 800.0f, 10.0f), FRotator::ZeroRotator);
	if (Hazard)
	{
		Hazard->SetActorScale3D(FVector(5.0f, 2.0f, 1.0f));
		Hazard->ActivateHazardEvent(EMCHazardType::Fire, 0.8f, 0.0f);
	}

	AMCSabotageDevice* Sabotage = GetWorld()->SpawnActor<AMCSabotageDevice>(SabotageDeviceClass, FVector(-250.0f, 420.0f, 70.0f), FRotator(0.0f, 180.0f, 0.0f));
	if (Sabotage)
	{
		Sabotage->ConfigurePrototypeSabotage(EMCSabotageType::LockdownShutter, MainExtraction, Hazard, MallModule);
	}

	const FVector LootLocations[] = {
		FVector(-900.0f, -250.0f, 80.0f),
		FVector(-900.0f, 0.0f, 80.0f),
		FVector(-900.0f, 250.0f, 80.0f),
		FVector(-650.0f, 150.0f, 80.0f)
	};

	const EMCHighValueLootPreset LootPresets[] = {
		EMCHighValueLootPreset::LuxuryElectronics,
		EMCHighValueLootPreset::GiantTelevision,
		EMCHighValueLootPreset::SafeBox,
		EMCHighValueLootPreset::JewelryCase
	};

	for (int32 Index = 0; Index < UE_ARRAY_COUNT(LootLocations); ++Index)
	{
		if (AMCLootItem* Loot = GetWorld()->SpawnActor<AMCLootItem>(LootItemClass, LootLocations[Index], FRotator::ZeroRotator))
		{
			Loot->ApplyHighValuePreset(LootPresets[Index]);
		}
	}

	AMCMatchDirector* Director = GetWorld()->SpawnActor<AMCMatchDirector>(MatchDirectorClass, FVector::ZeroVector, FRotator::ZeroRotator);
	if (Director)
	{
		Director->RegisterExtractionZone(MainExtraction);
		Director->RegisterExtractionZone(BackupExtraction);
		Director->RegisterHazardVolume(Hazard);
	}
}

FTransform AMCGameMode::GetPrototypeSpawnTransform()
{
	static const FVector SpawnLocations[] = {
		FVector(-350.0f, -250.0f, 110.0f),
		FVector(0.0f, -250.0f, 110.0f),
		FVector(350.0f, -250.0f, 110.0f),
		FVector(0.0f, 150.0f, 110.0f)
	};

	const FVector Location = SpawnLocations[PrototypeSpawnIndex % UE_ARRAY_COUNT(SpawnLocations)];
	++PrototypeSpawnIndex;
	return FTransform(FRotator(0.0f, 90.0f, 0.0f), Location, FVector::OneVector);
}
