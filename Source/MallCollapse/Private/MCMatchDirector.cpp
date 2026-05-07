#include "MCMatchDirector.h"

#include "MCExtractionZone.h"
#include "MCGameState.h"
#include "EngineUtils.h"
#include "Engine/World.h"

AMCMatchDirector::AMCMatchDirector()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AMCMatchDirector::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	if (bAutoFindExtractionZones)
	{
		AutoFindExtractionZones();
	}

	if (const AMCGameState* MallGameState = GetMallGameState())
	{
		ApplyPhasePlan(MallGameState->GetMatchPhase());
	}
}

void AMCMatchDirector::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority())
	{
		return;
	}

	const AMCGameState* MallGameState = GetMallGameState();
	if (!MallGameState)
	{
		return;
	}

	const EMCMatchPhase CurrentPhase = MallGameState->GetMatchPhase();
	if (CurrentPhase != LastAppliedPhase)
	{
		ApplyPhasePlan(CurrentPhase);
	}
}

void AMCMatchDirector::RegisterExtractionZone(AMCExtractionZone* ExtractionZone)
{
	if (!HasAuthority() || !ExtractionZone)
	{
		return;
	}

	ExtractionZones.AddUnique(ExtractionZone);
}

void AMCMatchDirector::ApplyPhasePlan(EMCMatchPhase MatchPhase)
{
	if (!HasAuthority())
	{
		return;
	}

	LastAppliedPhase = MatchPhase;

	switch (MatchPhase)
	{
	case EMCMatchPhase::Entry:
		ApplyEntryPlan();
		break;
	case EMCMatchPhase::Instability:
		ApplyInstabilityPlan();
		break;
	case EMCMatchPhase::CollapseCascade:
		ApplyCollapseCascadePlan();
		break;
	case EMCMatchPhase::FinalExtraction:
		ApplyFinalExtractionPlan();
		break;
	case EMCMatchPhase::MatchEnded:
		ApplyMatchEndedPlan();
		break;
	default:
		break;
	}

	HandlePhasePlanApplied(MatchPhase);
}

AMCGameState* AMCMatchDirector::GetMallGameState() const
{
	const UWorld* World = GetWorld();
	return World ? Cast<AMCGameState>(World->GetGameState()) : nullptr;
}

void AMCMatchDirector::AutoFindExtractionZones()
{
	ExtractionZones.Reset();

	if (!GetWorld())
	{
		return;
	}

	for (TActorIterator<AMCExtractionZone> It(GetWorld()); It; ++It)
	{
		ExtractionZones.Add(*It);
	}
}

void AMCMatchDirector::ApplyEntryPlan()
{
	for (int32 Index = 0; Index < ExtractionZones.Num(); ++Index)
	{
		if (AMCExtractionZone* Zone = ExtractionZones[Index])
		{
			const bool bBackupExit = bEntryStartsWithUnknownBackupExits && Index > 0;
			Zone->SetExtractionState(bBackupExit ? EMCExtractionState::Unknown : EMCExtractionState::Available);
		}
	}
}

void AMCMatchDirector::ApplyInstabilityPlan()
{
	for (AMCExtractionZone* Zone : ExtractionZones)
	{
		if (Zone && Zone->GetExtractionState() == EMCExtractionState::Unknown)
		{
			Zone->SetExtractionState(EMCExtractionState::Available);
		}
	}
}

void AMCMatchDirector::ApplyCollapseCascadePlan()
{
	TArray<AMCExtractionZone*> ShuffledZones;
	for (AMCExtractionZone* Zone : ExtractionZones)
	{
		if (Zone)
		{
			ShuffledZones.Add(Zone);
		}
	}

	ShuffleExtractionZones(ShuffledZones);

	for (int32 Index = 0; Index < ShuffledZones.Num(); ++Index)
	{
		const bool bShouldFailSoon = Index % 2 == 0;
		ShuffledZones[Index]->SetExtractionState(bShouldFailSoon ? EMCExtractionState::FailingSoon : EMCExtractionState::Available);
	}
}

void AMCMatchDirector::ApplyFinalExtractionPlan()
{
	TArray<AMCExtractionZone*> ShuffledZones;
	for (AMCExtractionZone* Zone : ExtractionZones)
	{
		if (Zone)
		{
			ShuffledZones.Add(Zone);
		}
	}

	ShuffleExtractionZones(ShuffledZones);

	const int32 AvailableCount = FMath::Clamp(MinFinalAvailableExits, 1, ShuffledZones.Num());
	for (int32 Index = 0; Index < ShuffledZones.Num(); ++Index)
	{
		ShuffledZones[Index]->SetExtractionState(Index < AvailableCount ? EMCExtractionState::Available : EMCExtractionState::Failed);
	}
}

void AMCMatchDirector::ApplyMatchEndedPlan()
{
	for (AMCExtractionZone* Zone : ExtractionZones)
	{
		if (Zone)
		{
			Zone->SetExtractionState(EMCExtractionState::Failed);
		}
	}
}

void AMCMatchDirector::ShuffleExtractionZones(TArray<AMCExtractionZone*>& ZonesToShuffle) const
{
	FRandomStream RandomStream(RandomSeed + static_cast<int32>(LastAppliedPhase) * 97);
	for (int32 Index = ZonesToShuffle.Num() - 1; Index > 0; --Index)
	{
		const int32 SwapIndex = RandomStream.RandRange(0, Index);
		ZonesToShuffle.Swap(Index, SwapIndex);
	}
}
