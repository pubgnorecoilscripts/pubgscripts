#include "MCPlayerController.h"

#include "CollisionQueryParams.h"
#include "MCPingActor.h"
#include "Engine/World.h"

AMCPlayerController::AMCPlayerController()
{
	PingActorClass = AMCPingActor::StaticClass();
}

void AMCPlayerController::CreatePingFromView(EMCPingType PingType)
{
	if (HasAuthority())
	{
		ProcessCreatePingFromView(PingType);
	}
	else
	{
		ServerCreatePingFromView(PingType);
	}
}

void AMCPlayerController::ServerCreatePing_Implementation(EMCPingType PingType, FVector_NetQuantize Location, AActor* TargetActor)
{
	CreatePingAtLocation(PingType, FVector(Location), TargetActor);
}

void AMCPlayerController::ServerCreatePingFromView_Implementation(EMCPingType PingType)
{
	ProcessCreatePingFromView(PingType);
}

bool AMCPlayerController::TracePingFromView(FVector& OutLocation, AActor*& OutTargetActor) const
{
	const APawn* ControlledPawn = GetPawn();
	const UWorld* World = GetWorld();
	if (!ControlledPawn || !World)
	{
		return false;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector TraceStart = ViewLocation;
	const FVector TraceEnd = TraceStart + ViewRotation.Vector() * MaxPingDistance;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(MallCollapsePingTrace), false, ControlledPawn);

	FHitResult HitResult;
	const bool bHit = World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, PingTraceChannel, QueryParams);
	OutLocation = bHit ? HitResult.ImpactPoint : TraceEnd;
	OutTargetActor = bHit ? HitResult.GetActor() : nullptr;
	return true;
}

void AMCPlayerController::CreatePingAtLocation(EMCPingType PingType, const FVector& Location, AActor* TargetActor)
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

void AMCPlayerController::ProcessCreatePingFromView(EMCPingType PingType)
{
	FVector PingLocation;
	AActor* TargetActor = nullptr;
	if (TracePingFromView(PingLocation, TargetActor))
	{
		CreatePingAtLocation(PingType, PingLocation, TargetActor);
	}
}
