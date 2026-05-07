#include "MCExtractionZone.h"

#include "MCCarryComponent.h"
#include "MCGameState.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

AMCExtractionZone::AMCExtractionZone()
{
	bReplicates = true;

	ExtractionBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("ExtractionBounds"));
	SetRootComponent(ExtractionBounds);
	ExtractionBounds->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExtractionBounds->OnComponentBeginOverlap.AddDynamic(this, &AMCExtractionZone::HandleBeginOverlap);
	ExtractionBounds->OnComponentEndOverlap.AddDynamic(this, &AMCExtractionZone::HandleEndOverlap);
}

void AMCExtractionZone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMCExtractionZone, ExtractionState);
	DOREPLIFETIME(AMCExtractionZone, LastPanicEventType);
	DOREPLIFETIME(AMCExtractionZone, PanicEventSequence);
}

bool AMCExtractionZone::TryExtractActor(AActor* ActorToExtract)
{
	if (!HasAuthority() || !ActorToExtract || ExtractionState != EMCExtractionState::Available)
	{
		return false;
	}

	UMCCarryComponent* CarryComponent = ActorToExtract->FindComponentByClass<UMCCarryComponent>();
	const int32 ExtractedValue = CarryComponent ? CarryComponent->ExtractAllLoot() : 0;

	UWorld* World = GetWorld();
	AMCGameState* MallGameState = World ? Cast<AMCGameState>(World->GetGameState()) : nullptr;
	if (MallGameState)
	{
		APawn* ExtractedPawn = Cast<APawn>(ActorToExtract);
		if (ExtractedPawn && ExtractedPawn->GetController())
		{
			MallGameState->RecordPlayerExtraction(ExtractedPawn->GetController(), ExtractedValue);
		}
		else
		{
			MallGameState->AddExtractedValue(ExtractedValue);
		}
	}

	if (bHideExtractedActors)
	{
		ActorToExtract->SetActorHiddenInGame(true);
	}

	if (bDisableExtractedActorCollision)
	{
		ActorToExtract->SetActorEnableCollision(false);
	}

	OnActorExtracted.Broadcast(ActorToExtract, ExtractedValue);
	return true;
}

void AMCExtractionZone::SetExtractionState(EMCExtractionState NewState)
{
	if (!HasAuthority())
	{
		return;
	}

	ExtractionState = NewState;
	HandleExtractionStateChanged();
}

void AMCExtractionZone::TriggerExtractionPanicEvent(EMCExtractionPanicEventType PanicEventType, float DurationSeconds)
{
	if (!HasAuthority())
	{
		return;
	}

	LastPanicEventType = PanicEventType;
	++PanicEventSequence;
	HandleExtractionPanicEvent(LastPanicEventType, PanicEventSequence);

	StateBeforeTemporaryPanicEvent = ExtractionState;
	const float EffectiveDuration = DurationSeconds > 0.0f ? DurationSeconds : DefaultPanicEventDurationSeconds;

	switch (PanicEventType)
	{
	case EMCExtractionPanicEventType::ElevatorDelay:
		SetExtractionState(EMCExtractionState::TemporarilyBlocked);
		GetWorldTimerManager().SetTimer(TemporaryStateTimerHandle, this, &AMCExtractionZone::RestoreTemporaryExtractionState, EffectiveDuration, false);
		break;
	case EMCExtractionPanicEventType::FakeExtractionAlert:
		// The state intentionally does not change; the announcement/UI layer makes players question the exit.
		break;
	case EMCExtractionPanicEventType::EmergencyReroute:
		SetExtractionState(EMCExtractionState::RequiresPower);
		GetWorldTimerManager().SetTimer(TemporaryStateTimerHandle, this, &AMCExtractionZone::RestoreTemporaryExtractionState, EffectiveDuration, false);
		break;
	case EMCExtractionPanicEventType::CollapsingPath:
		SetExtractionState(EMCExtractionState::FailingSoon);
		{
			FTimerDelegate FailDelegate;
			FailDelegate.BindUObject(this, &AMCExtractionZone::SetExtractionState, EMCExtractionState::Failed);
			GetWorldTimerManager().SetTimer(TemporaryStateTimerHandle, FailDelegate, EffectiveDuration, false);
		}
		break;
	case EMCExtractionPanicEventType::LastSecondCountdown:
		SetExtractionState(EMCExtractionState::FailingSoon);
		GetWorldTimerManager().SetTimer(TemporaryStateTimerHandle, this, &AMCExtractionZone::RestoreTemporaryExtractionState, EffectiveDuration, false);
		break;
	default:
		break;
	}
}

void AMCExtractionZone::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority() && OtherActor && OtherActor != this)
	{
		BeginExtractionAttempt(OtherActor);
	}
}

void AMCExtractionZone::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (HasAuthority() && OtherActor)
	{
		CancelExtractionAttempt(OtherActor);
	}
}

void AMCExtractionZone::OnRep_ExtractionState()
{
	HandleExtractionStateChanged();
}

void AMCExtractionZone::OnRep_LastPanicEvent()
{
	HandleExtractionPanicEvent(LastPanicEventType, PanicEventSequence);
}

void AMCExtractionZone::BeginExtractionAttempt(AActor* ActorToExtract)
{
	if (!ActorToExtract || ExtractionState != EMCExtractionState::Available)
	{
		return;
	}

	if (ExtractionDelaySeconds <= 0.0f)
	{
		TryExtractActor(ActorToExtract);
		return;
	}

	TWeakObjectPtr<AActor> ActorKey(ActorToExtract);
	if (PendingExtractionActors.Contains(ActorKey))
	{
		return;
	}

	PendingExtractionActors.Add(ActorKey);

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &AMCExtractionZone::CompleteDelayedExtraction, ActorToExtract);

	FTimerHandle& TimerHandle = PendingExtractionTimers.FindOrAdd(ActorKey);
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, ExtractionDelaySeconds, false);
	HandleExtractionCountdownStarted(ActorToExtract, ExtractionDelaySeconds);
}

void AMCExtractionZone::CancelExtractionAttempt(AActor* ActorToExtract)
{
	TWeakObjectPtr<AActor> ActorKey(ActorToExtract);
	if (!PendingExtractionActors.Remove(ActorKey))
	{
		return;
	}

	if (FTimerHandle* TimerHandle = PendingExtractionTimers.Find(ActorKey))
	{
		GetWorldTimerManager().ClearTimer(*TimerHandle);
		PendingExtractionTimers.Remove(ActorKey);
	}

	HandleExtractionCountdownCancelled(ActorToExtract);
}

void AMCExtractionZone::CompleteDelayedExtraction(AActor* ActorToExtract)
{
	TWeakObjectPtr<AActor> ActorKey(ActorToExtract);
	PendingExtractionActors.Remove(ActorKey);
	PendingExtractionTimers.Remove(ActorKey);

	TryExtractActor(ActorToExtract);
}

void AMCExtractionZone::RestoreTemporaryExtractionState()
{
	if (HasAuthority() && ExtractionState != EMCExtractionState::Failed)
	{
		SetExtractionState(StateBeforeTemporaryPanicEvent);
	}
}
