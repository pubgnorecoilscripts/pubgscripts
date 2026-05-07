#include "MCHazardVolume.h"

#include "MCMallModuleStateActor.h"
#include "MCPanicComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AMCHazardVolume::AMCHazardVolume()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	HazardBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("HazardBounds"));
	SetRootComponent(HazardBounds);
	HazardBounds->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	HazardBounds->OnComponentBeginOverlap.AddDynamic(this, &AMCHazardVolume::HandleBeginOverlap);
	HazardBounds->OnComponentEndOverlap.AddDynamic(this, &AMCHazardVolume::HandleEndOverlap);
}

void AMCHazardVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority() || !bActive)
	{
		return;
	}

	ApplyLinkedModuleDamage(DeltaSeconds);

	for (auto It = Occupants.CreateIterator(); It; ++It)
	{
		AActor* Occupant = It->Get();
		if (!Occupant)
		{
			It.RemoveCurrent();
			continue;
		}

		if (UMCPanicComponent* PanicComponent = Occupant->FindComponentByClass<UMCPanicComponent>())
		{
			PanicComponent->AddPanic(PanicPerSecond * Intensity * DeltaSeconds);
		}

		if (bDamagePlayers && DamagePerSecond > 0.0f)
		{
			UGameplayStatics::ApplyDamage(Occupant, DamagePerSecond * Intensity * DeltaSeconds, nullptr, this, nullptr);
		}
	}
}

void AMCHazardVolume::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMCHazardVolume, HazardType);
	DOREPLIFETIME(AMCHazardVolume, bActive);
	DOREPLIFETIME(AMCHazardVolume, Intensity);
}

void AMCHazardVolume::SetHazardActive(bool bNewActive)
{
	if (!HasAuthority())
	{
		return;
	}

	bActive = bNewActive;
	HandleHazardStateChanged();
}

void AMCHazardVolume::SetIntensity(float NewIntensity)
{
	if (!HasAuthority())
	{
		return;
	}

	Intensity = FMath::Max(0.0f, NewIntensity);
}

void AMCHazardVolume::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority() && OtherActor && OtherActor != this)
	{
		Occupants.Add(OtherActor);
	}
}

void AMCHazardVolume::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (HasAuthority() && OtherActor)
	{
		Occupants.Remove(OtherActor);
	}
}

void AMCHazardVolume::OnRep_HazardState()
{
	HandleHazardStateChanged();
}

void AMCHazardVolume::ApplyLinkedModuleDamage(float DeltaSeconds)
{
	const float DamageAmount = ModuleDamagePerSecond * Intensity * DeltaSeconds;
	if (DamageAmount <= 0.0f)
	{
		return;
	}

	for (AMCMallModuleStateActor* Module : LinkedMallModules)
	{
		if (!Module)
		{
			continue;
		}

		switch (HazardType)
		{
		case EMCHazardType::Fire:
		case EMCHazardType::Smoke:
			Module->ApplyFireDamage(DamageAmount);
			break;
		case EMCHazardType::Flood:
			Module->ApplyWaterDamage(DamageAmount);
			break;
		case EMCHazardType::Electrical:
			Module->ApplyElectricalInstability(DamageAmount);
			break;
		case EMCHazardType::StructuralCollapse:
			Module->ApplyOverloadStress(DamageAmount);
			break;
		case EMCHazardType::SecurityLockdown:
			Module->ApplyElectricalInstability(DamageAmount * 0.5f);
			break;
		default:
			break;
		}
	}
}
