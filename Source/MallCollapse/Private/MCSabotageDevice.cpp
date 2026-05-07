#include "MCSabotageDevice.h"

#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

AMCSabotageDevice::AMCSabotageDevice()
{
	bReplicates = true;
}

void AMCSabotageDevice::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMCSabotageDevice, SabotageType);
	DOREPLIFETIME(AMCSabotageDevice, bActivated);
	DOREPLIFETIME(AMCSabotageDevice, LastActivationTimeSeconds);
	DOREPLIFETIME(AMCSabotageDevice, LastInstigatorController);
}

bool AMCSabotageDevice::ActivateSabotage(AController* InstigatorController)
{
	if (!HasAuthority() || !CanActivateSabotage())
	{
		return false;
	}

	bActivated = true;
	LastInstigatorController = InstigatorController;
	LastActivationTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

	OnSabotageActivated.Broadcast(SabotageType, InstigatorController);
	HandleSabotageActivated(InstigatorController);
	ScheduleResetIfNeeded();
	return true;
}

bool AMCSabotageDevice::CanActivateSabotage() const
{
	if (bOneShot && bActivated)
	{
		return false;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return true;
	}

	const float TimeSinceActivation = World->GetTimeSeconds() - LastActivationTimeSeconds;
	return TimeSinceActivation >= CooldownSeconds;
}

void AMCSabotageDevice::ResetSabotage()
{
	if (!HasAuthority())
	{
		return;
	}

	bActivated = false;
	HandleSabotageReset();
}

bool AMCSabotageDevice::CanInteract_Implementation(APawn* InteractingPawn) const
{
	return InteractingPawn && CanActivateSabotage();
}

void AMCSabotageDevice::Interact_Implementation(APawn* InteractingPawn)
{
	if (!HasAuthority() || !InteractingPawn)
	{
		return;
	}

	ActivateSabotage(InteractingPawn->GetController());
}

FText AMCSabotageDevice::GetInteractionText_Implementation() const
{
	return NSLOCTEXT("MallCollapse", "ActivateSabotage", "Activate Sabotage");
}

void AMCSabotageDevice::OnRep_SabotageState()
{
	if (bActivated)
	{
		OnSabotageActivated.Broadcast(SabotageType, LastInstigatorController);
		HandleSabotageActivated(LastInstigatorController);
	}
	else
	{
		HandleSabotageReset();
	}
}

void AMCSabotageDevice::ScheduleResetIfNeeded()
{
	if (bOneShot || ActiveDurationSeconds <= 0.0f || !GetWorld())
	{
		return;
	}

	FTimerHandle ResetTimerHandle;
	GetWorldTimerManager().SetTimer(ResetTimerHandle, this, &AMCSabotageDevice::ResetSabotage, ActiveDurationSeconds, false);
}
