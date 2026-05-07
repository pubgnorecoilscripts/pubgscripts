#include "MCExtractionZone.h"

#include "MCCarryComponent.h"
#include "MCGameState.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

AMCExtractionZone::AMCExtractionZone()
{
	bReplicates = true;

	ExtractionBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("ExtractionBounds"));
	SetRootComponent(ExtractionBounds);
	ExtractionBounds->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExtractionBounds->OnComponentBeginOverlap.AddDynamic(this, &AMCExtractionZone::HandleBeginOverlap);
}

void AMCExtractionZone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMCExtractionZone, ExtractionState);
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
		MallGameState->AddExtractedValue(ExtractedValue);
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

void AMCExtractionZone::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority() && OtherActor && OtherActor != this)
	{
		TryExtractActor(OtherActor);
	}
}

void AMCExtractionZone::OnRep_ExtractionState()
{
	HandleExtractionStateChanged();
}
