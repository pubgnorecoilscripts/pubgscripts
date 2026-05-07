#include "MCGameState.h"

#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"
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
	DOREPLIFETIME(AMCGameState, ExtractionResults);
	DOREPLIFETIME(AMCGameState, LastAnnouncement);
	DOREPLIFETIME(AMCGameState, LastAtmosphereCue);
	DOREPLIFETIME(AMCGameState, AnnouncementSequence);
	DOREPLIFETIME(AMCGameState, AtmosphereSequence);
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

void AMCGameState::RecordPlayerExtraction(AController* ExtractingController, int32 Value)
{
	if (!HasAuthority() || !ExtractingController)
	{
		return;
	}

	AddExtractedValue(Value);

	FMCPlayerExtractionResult Result;
	Result.PlayerState = ExtractingController->PlayerState;
	Result.PlayerName = ExtractingController->PlayerState ? ExtractingController->PlayerState->GetPlayerName() : FString(TEXT("Unknown"));
	Result.ExtractedValue = FMath::Max(0, Value);
	Result.MatchTimeSeconds = MatchElapsedSeconds;
	Result.MatchPhase = MatchPhase;
	ExtractionResults.Add(Result);
}

void AMCGameState::BroadcastMallAnnouncement(EMCAnnouncementType AnnouncementType, const FString& Message, bool bFake, float Intensity)
{
	if (!HasAuthority())
	{
		return;
	}

	LastAnnouncement.SequenceId = ++AnnouncementSequence;
	LastAnnouncement.AnnouncementType = AnnouncementType;
	LastAnnouncement.Message = Message;
	LastAnnouncement.bFake = bFake;
	LastAnnouncement.Intensity = FMath::Max(0.0f, Intensity);
	LastAnnouncement.MatchTimeSeconds = MatchElapsedSeconds;
	OnMallAnnouncement.Broadcast(LastAnnouncement);
}

void AMCGameState::BroadcastAtmosphereCue(EMCAtmosphereCueType CueType, float Intensity, FVector Location, float Radius)
{
	if (!HasAuthority())
	{
		return;
	}

	LastAtmosphereCue.SequenceId = ++AtmosphereSequence;
	LastAtmosphereCue.CueType = CueType;
	LastAtmosphereCue.Intensity = FMath::Max(0.0f, Intensity);
	LastAtmosphereCue.Location = Location;
	LastAtmosphereCue.Radius = FMath::Max(0.0f, Radius);
	LastAtmosphereCue.MatchTimeSeconds = MatchElapsedSeconds;
	OnAtmosphereCue.Broadcast(LastAtmosphereCue);
}

void AMCGameState::OnRep_MatchPhase()
{
	OnMatchPhaseChanged.Broadcast(MatchPhase);
}

void AMCGameState::OnRep_LastAnnouncement()
{
	OnMallAnnouncement.Broadcast(LastAnnouncement);
}

void AMCGameState::OnRep_LastAtmosphereCue()
{
	OnAtmosphereCue.Broadcast(LastAtmosphereCue);
}
