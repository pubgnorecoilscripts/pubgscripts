#include "MCSabotageDevice.h"

#include "MCExtractionZone.h"
#include "MCGameState.h"
#include "MCHazardVolume.h"
#include "MCMallModuleStateActor.h"
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

	ApplySabotageEffects(InstigatorController);
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

void AMCSabotageDevice::ApplySabotageEffects(AController* InstigatorController)
{
	switch (SabotageType)
	{
	case EMCSabotageType::LockdownShutter:
	case EMCSabotageType::SecurityShutterLockdown:
		ApplyExtractionLockdown();
		break;
	case EMCSabotageType::PowerShutdown:
	case EMCSabotageType::BreakerBlackout:
		ApplyBreakerBlackout();
		break;
	case EMCSabotageType::ReverseEscalator:
	case EMCSabotageType::EscalatorShutdown:
		ApplyEscalatorShutdown();
		break;
	case EMCSabotageType::FloodValve:
	case EMCSabotageType::SprinklerFlooding:
		ApplySprinklerFlooding();
		break;
	case EMCSabotageType::FakeAnnouncement:
	case EMCSabotageType::FalseEmergencyAlarm:
		ApplyFalseEmergencyAlarm();
		break;
	default:
		break;
	}
}

void AMCSabotageDevice::ApplyExtractionLockdown()
{
	for (AMCExtractionZone* Zone : LinkedExtractionZones)
	{
		if (Zone)
		{
			Zone->TriggerExtractionPanicEvent(EMCExtractionPanicEventType::ElevatorDelay, ActiveDurationSeconds);
		}
	}

	if (AMCGameState* MallGameState = GetMallGameState())
	{
		MallGameState->BroadcastMallAnnouncement(EMCAnnouncementType::SecurityAlert, TEXT("Security shutters temporarily engaged. Please betray each other in an orderly fashion."), false, 1.0f);
		MallGameState->BroadcastAtmosphereCue(EMCAtmosphereCueType::EmergencySiren, 1.0f, GetActorLocation(), 1200.0f);
	}
}

void AMCSabotageDevice::ApplyBreakerBlackout()
{
	for (AMCMallModuleStateActor* Module : LinkedMallModules)
	{
		if (Module)
		{
			Module->ApplyElectricalInstability(ModuleStressAmount);
		}
	}

	if (AMCGameState* MallGameState = GetMallGameState())
	{
		MallGameState->BroadcastMallAnnouncement(EMCAnnouncementType::EmergencyWarning, TEXT("Local lighting failure detected. This is not a stealth tutorial."), false, 1.2f);
		MallGameState->BroadcastAtmosphereCue(EMCAtmosphereCueType::Blackout, 1.2f, GetActorLocation(), 1800.0f);
	}
}

void AMCSabotageDevice::ApplyEscalatorShutdown()
{
	for (AMCMallModuleStateActor* Module : LinkedMallModules)
	{
		if (Module)
		{
			Module->ApplyOverloadStress(ModuleStressAmount * 0.5f);
		}
	}

	if (AMCGameState* MallGameState = GetMallGameState())
	{
		MallGameState->BroadcastMallAnnouncement(EMCAnnouncementType::SecurityAlert, TEXT("Escalator service interruption. Heavy shoppers should reconsider their ambitions."), false, 0.9f);
		MallGameState->BroadcastAtmosphereCue(EMCAtmosphereCueType::EnvironmentalPanic, 0.9f, GetActorLocation(), 1200.0f);
	}
}

void AMCSabotageDevice::ApplySprinklerFlooding()
{
	for (AMCHazardVolume* Hazard : LinkedHazardVolumes)
	{
		if (Hazard)
		{
			Hazard->ActivateHazardEvent(EMCHazardType::Flood, 1.0f, ActiveDurationSeconds);
		}
	}

	for (AMCMallModuleStateActor* Module : LinkedMallModules)
	{
		if (Module)
		{
			Module->ApplyWaterDamage(ModuleStressAmount);
		}
	}

	if (AMCGameState* MallGameState = GetMallGameState())
	{
		MallGameState->BroadcastMallAnnouncement(EMCAnnouncementType::MalfunctioningAI, TEXT("Sprinklers activated for your comfort and electrical danger."), false, 1.1f);
		MallGameState->BroadcastAtmosphereCue(EMCAtmosphereCueType::EnvironmentalPanic, 1.1f, GetActorLocation(), 1600.0f);
	}
}

void AMCSabotageDevice::ApplyFalseEmergencyAlarm()
{
	if (AMCGameState* MallGameState = GetMallGameState())
	{
		MallGameState->BroadcastMallAnnouncement(EMCAnnouncementType::FakeEvacuation, TEXT("Immediate evacuation recommended through the most suspicious nearby route."), true, 1.0f);
		MallGameState->BroadcastAtmosphereCue(EMCAtmosphereCueType::EmergencySiren, 1.0f, GetActorLocation(), 2000.0f);
	}
}

AMCGameState* AMCSabotageDevice::GetMallGameState() const
{
	const UWorld* World = GetWorld();
	return World ? Cast<AMCGameState>(World->GetGameState()) : nullptr;
}
