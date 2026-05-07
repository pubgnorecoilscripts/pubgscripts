#include "MCPlayerController.h"

#include "MCPingActor.h"
#include "Engine/World.h"

AMCPlayerController::AMCPlayerController()
{
	PingActorClass = AMCPingActor::StaticClass();
}

void AMCPlayerController::ServerCreatePing_Implementation(EMCPingType PingType, FVector_NetQuantize Location, AActor* TargetActor)
{
	if (!PingActorClass || !GetWorld())
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetPawn();

	AMCPingActor* PingActor = GetWorld()->SpawnActor<AMCPingActor>(PingActorClass, FVector(Location), FRotator::ZeroRotator, SpawnParams);
	if (PingActor)
	{
		PingActor->InitializePing(PingType, this, TargetActor, PingLifetimeSeconds);
	}
}
