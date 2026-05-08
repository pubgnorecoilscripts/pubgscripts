#include "MCPanicComponent.h"

#include "Net/UnrealNetwork.h"

UMCPanicComponent::UMCPanicComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UMCPanicComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bPassiveRecoveryEnabled && GetOwner() && GetOwner()->HasAuthority() && Panic > 0.0f)
	{
		RecoverPanic(PassiveRecoveryPerSecond * DeltaTime);
	}
}

void UMCPanicComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMCPanicComponent, Panic);
}

void UMCPanicComponent::AddPanic(float Amount)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || Amount <= 0.0f)
	{
		return;
	}

	const float PreviousPanic = Panic;
	Panic = FMath::Clamp(Panic + Amount, 0.0f, MaxPanic);

	if (!FMath::IsNearlyEqual(PreviousPanic, Panic))
	{
		BroadcastPanicChanged();
	}
}

void UMCPanicComponent::RecoverPanic(float Amount)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || Amount <= 0.0f)
	{
		return;
	}

	const float PreviousPanic = Panic;
	Panic = FMath::Clamp(Panic - Amount, 0.0f, MaxPanic);

	if (!FMath::IsNearlyEqual(PreviousPanic, Panic))
	{
		BroadcastPanicChanged();
	}
}

EMCPanicTier UMCPanicComponent::GetPanicTier() const
{
	if (Panic >= 75.0f)
	{
		return EMCPanicTier::Panicked;
	}

	if (Panic >= 45.0f)
	{
		return EMCPanicTier::Stressed;
	}

	if (Panic >= 15.0f)
	{
		return EMCPanicTier::Alert;
	}

	return EMCPanicTier::Calm;
}

float UMCPanicComponent::GetMovementStabilityMultiplier() const
{
	switch (GetPanicTier())
	{
	case EMCPanicTier::Calm:
		return 1.0f;
	case EMCPanicTier::Alert:
		return 0.98f;
	case EMCPanicTier::Stressed:
		return 0.92f;
	case EMCPanicTier::Panicked:
		return 0.82f;
	default:
		return 1.0f;
	}
}

float UMCPanicComponent::GetInteractionFumbleChance() const
{
	switch (GetPanicTier())
	{
	case EMCPanicTier::Calm:
		return 0.0f;
	case EMCPanicTier::Alert:
		return 0.03f;
	case EMCPanicTier::Stressed:
		return 0.08f;
	case EMCPanicTier::Panicked:
		return 0.16f;
	default:
		return 0.0f;
	}
}

void UMCPanicComponent::OnRep_Panic()
{
	BroadcastPanicChanged();
}

void UMCPanicComponent::BroadcastPanicChanged()
{
	OnPanicChanged.Broadcast(Panic, GetPanicTier());
}
