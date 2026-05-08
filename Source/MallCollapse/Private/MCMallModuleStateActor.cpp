#include "MCMallModuleStateActor.h"

#include "Net/UnrealNetwork.h"

AMCMallModuleStateActor::AMCMallModuleStateActor()
{
	bReplicates = true;
}

void AMCMallModuleStateActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMCMallModuleStateActor, IntegrityState);
	DOREPLIFETIME(AMCMallModuleStateActor, FireDamage);
	DOREPLIFETIME(AMCMallModuleStateActor, WaterDamage);
	DOREPLIFETIME(AMCMallModuleStateActor, OverloadStress);
	DOREPLIFETIME(AMCMallModuleStateActor, ElectricalInstability);
}

void AMCMallModuleStateActor::ApplyFireDamage(float Amount)
{
	if (!HasAuthority() || Amount <= 0.0f)
	{
		return;
	}

	FireDamage = FMath::Clamp(FireDamage + Amount, 0.0f, 100.0f);
	RecalculateIntegrityState();
}

void AMCMallModuleStateActor::ApplyWaterDamage(float Amount)
{
	if (!HasAuthority() || Amount <= 0.0f)
	{
		return;
	}

	WaterDamage = FMath::Clamp(WaterDamage + Amount, 0.0f, 100.0f);
	RecalculateIntegrityState();
}

void AMCMallModuleStateActor::ApplyOverloadStress(float Amount)
{
	if (!HasAuthority() || Amount <= 0.0f)
	{
		return;
	}

	OverloadStress = FMath::Clamp(OverloadStress + Amount, 0.0f, 100.0f);
	RecalculateIntegrityState();
}

void AMCMallModuleStateActor::ApplyElectricalInstability(float Amount)
{
	if (!HasAuthority() || Amount <= 0.0f)
	{
		return;
	}

	ElectricalInstability = FMath::Clamp(ElectricalInstability + Amount, 0.0f, 100.0f);
	RecalculateIntegrityState();
}

void AMCMallModuleStateActor::SetIntegrityState(EMCModuleIntegrityState NewState)
{
	if (!HasAuthority() || IntegrityState == NewState)
	{
		return;
	}

	IntegrityState = NewState;
	OnIntegrityStateChanged.Broadcast(IntegrityState);
	HandleIntegrityStateChanged();
}

void AMCMallModuleStateActor::OnRep_IntegrityState()
{
	OnIntegrityStateChanged.Broadcast(IntegrityState);
	HandleIntegrityStateChanged();
}

void AMCMallModuleStateActor::RecalculateIntegrityState()
{
	const float DamageScore = GetCombinedDamageScore();
	EMCModuleIntegrityState NewState = EMCModuleIntegrityState::Intact;

	if (DamageScore >= 90.0f)
	{
		NewState = EMCModuleIntegrityState::Blocked;
	}
	else if (DamageScore >= 70.0f)
	{
		NewState = EMCModuleIntegrityState::PartiallyCollapsed;
	}
	else if (DamageScore >= 45.0f)
	{
		NewState = EMCModuleIntegrityState::Hazardous;
	}
	else if (DamageScore >= 20.0f)
	{
		NewState = EMCModuleIntegrityState::Damaged;
	}

	SetIntegrityState(NewState);
}

float AMCMallModuleStateActor::GetCombinedDamageScore() const
{
	return FireDamage * 0.35f
		+ WaterDamage * 0.2f
		+ OverloadStress * 0.3f
		+ ElectricalInstability * 0.15f;
}
