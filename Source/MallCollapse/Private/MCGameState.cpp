#include "MCGameState.h"

#include "Net/UnrealNetwork.h"

AMCGameState::AMCGameState()
{
	bReplicates = true;
}

void AMCGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMCGameState, MatchPhase);
	DOREPLIFETIME(AMCGameState, MatchElapsedSeconds);
	DOREPLIFETIME(AMCGameState, PhaseElapsedSeconds);
	DOREPLIFETIME(AMCGameState, TotalExtractedValue);
}

void AMCGameState::SetMatchPhase(EMCMatchPhase NewPhase)
{
	if (!HasAuthority() || MatchPhase == NewPhase)
	{
		return;
	}

	MatchPhase = NewPhase;
	PhaseElapsedSeconds = 0.0f;
	OnMatchPhaseChanged.Broadcast(MatchPhase);
}

void AMCGameState::SetMatchClock(float NewMatchElapsedSeconds, float NewPhaseElapsedSeconds)
{
	if (!HasAuthority())
	{
		return;
	}

	MatchElapsedSeconds = FMath::Max(0.0f, NewMatchElapsedSeconds);
	PhaseElapsedSeconds = FMath::Max(0.0f, NewPhaseElapsedSeconds);
}

void AMCGameState::AddExtractedValue(int32 Value)
{
	if (!HasAuthority() || Value <= 0)
	{
		return;
	}

	TotalExtractedValue += Value;
}

void AMCGameState::OnRep_MatchPhase()
{
	OnMatchPhaseChanged.Broadcast(MatchPhase);
}
