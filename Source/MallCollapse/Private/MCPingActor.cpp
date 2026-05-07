#include "MCPingActor.h"

#include "Net/UnrealNetwork.h"

AMCPingActor::AMCPingActor()
{
	bReplicates = true;
	SetReplicateMovement(true);
}

void AMCPingActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetLifeSpan(DefaultLifetimeSeconds);
	}
}

void AMCPingActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMCPingActor, PingType);
	DOREPLIFETIME(AMCPingActor, OwningController);
	DOREPLIFETIME(AMCPingActor, TargetActor);
}

void AMCPingActor::InitializePing(EMCPingType NewPingType, AController* NewOwningController, AActor* NewTargetActor, float NewLifetimeSeconds)
{
	if (!HasAuthority())
	{
		return;
	}

	PingType = NewPingType;
	OwningController = NewOwningController;
	TargetActor = NewTargetActor;
	SetLifeSpan(NewLifetimeSeconds > 0.0f ? NewLifetimeSeconds : DefaultLifetimeSeconds);
	HandlePingDataChanged();
}

void AMCPingActor::OnRep_PingData()
{
	HandlePingDataChanged();
}
