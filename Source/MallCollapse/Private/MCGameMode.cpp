#include "MCGameMode.h"

#include "MCCharacter.h"
#include "MCGameState.h"
#include "MCPlayerController.h"

AMCGameMode::AMCGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	GameStateClass = AMCGameState::StaticClass();
	DefaultPawnClass = AMCCharacter::StaticClass();
	PlayerControllerClass = AMCPlayerController::StaticClass();
}

void AMCGameMode::BeginPlay()
{
	Super::BeginPlay();

	ChangePhase(EMCMatchPhase::Entry);
}

void AMCGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AdvanceMatchClock(DeltaSeconds);
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
