#include "MCCarryComponent.h"

#include "MCLootItem.h"
#include "MCPanicComponent.h"
#include "Net/UnrealNetwork.h"

UMCCarryComponent::UMCCarryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UMCCarryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner() || !GetOwner()->HasAuthority() || CurrentRiskPanicPerSecond <= 0.0f)
	{
		return;
	}

	if (UMCPanicComponent* PanicComponent = GetOwner()->FindComponentByClass<UMCPanicComponent>())
	{
		PanicComponent->AddPanic(CurrentRiskPanicPerSecond * DeltaTime);
	}
}

void UMCCarryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMCCarryComponent, CarryState);
	DOREPLIFETIME(UMCCarryComponent, CurrentCarryWeight);
	DOREPLIFETIME(UMCCarryComponent, CurrentCarryValue);
	DOREPLIFETIME(UMCCarryComponent, bDragging);
	DOREPLIFETIME(UMCCarryComponent, CurrentVisionObstruction);
	DOREPLIFETIME(UMCCarryComponent, CurrentCarriedNoiseRadius);
	DOREPLIFETIME(UMCCarryComponent, bCarryingTwoHandedLoot);
	DOREPLIFETIME(UMCCarryComponent, CurrentRiskPanicPerSecond);
	DOREPLIFETIME(UMCCarryComponent, CurrentLootMovementPenaltyMultiplier);
	DOREPLIFETIME(UMCCarryComponent, CarriedItems);
}

bool UMCCarryComponent::AddLoot(AMCLootItem* LootItem)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !LootItem || !LootItem->CanBePickedUp())
	{
		return false;
	}

	CarriedItems.Add(LootItem);
	LootItem->MarkCarried(GetOwner());

	RecalculateCarryState();
	return true;
}

bool UMCCarryComponent::DropLoot(AMCLootItem* LootItem)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !LootItem)
	{
		return false;
	}

	const int32 RemovedCount = CarriedItems.Remove(LootItem);
	if (RemovedCount <= 0)
	{
		return false;
	}

	LootItem->MarkDropped(GetDropLocation(0));
	RecalculateCarryState();
	return true;
}

void UMCCarryComponent::DropAllLoot()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	TArray<TObjectPtr<AMCLootItem>> ItemsToDrop = CarriedItems;
	CarriedItems.Reset();

	for (int32 Index = 0; Index < ItemsToDrop.Num(); ++Index)
	{
		if (AMCLootItem* LootItem = ItemsToDrop[Index])
		{
			LootItem->MarkDropped(GetDropLocation(Index));
		}
	}

	RecalculateCarryState();
}

int32 UMCCarryComponent::ExtractAllLoot()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return 0;
	}

	int32 ExtractedValue = 0;
	for (AMCLootItem* LootItem : CarriedItems)
	{
		if (!LootItem)
		{
			continue;
		}

		ExtractedValue += LootItem->GetLootDescriptor().Value;
		LootItem->MarkExtracted();
	}

	CarriedItems.Reset();
	RecalculateCarryState();
	return ExtractedValue;
}

void UMCCarryComponent::ServerDropAllLoot_Implementation()
{
	DropAllLoot();
}

void UMCCarryComponent::SetDragging(bool bNewDragging)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	bDragging = bNewDragging;
	RecalculateCarryState();
}

float UMCCarryComponent::GetMovementSpeedMultiplier() const
{
	switch (CarryState)
	{
	case EMCCarryState::Light:
		return 1.0f * CurrentLootMovementPenaltyMultiplier;
	case EMCCarryState::Encumbered:
		return 0.86f * CurrentLootMovementPenaltyMultiplier;
	case EMCCarryState::Overloaded:
		return 0.64f * CurrentLootMovementPenaltyMultiplier;
	case EMCCarryState::Dragging:
		return 0.45f * CurrentLootMovementPenaltyMultiplier;
	default:
		return CurrentLootMovementPenaltyMultiplier;
	}
}

void UMCCarryComponent::OnRep_CarryState()
{
	OnCarryStateChanged.Broadcast(CarryState);
}

void UMCCarryComponent::RecalculateCarryState()
{
	CurrentCarryWeight = 0.0f;
	CurrentCarryValue = 0;
	CurrentVisionObstruction = 0.0f;
	CurrentCarriedNoiseRadius = 0.0f;
	bCarryingTwoHandedLoot = false;
	CurrentRiskPanicPerSecond = 0.0f;
	CurrentLootMovementPenaltyMultiplier = 1.0f;

	for (const AMCLootItem* LootItem : CarriedItems)
	{
		if (!LootItem)
		{
			continue;
		}

		const FMCLootDescriptor Descriptor = LootItem->GetLootDescriptor();
		CurrentCarryWeight += Descriptor.Weight;
		CurrentCarryValue += Descriptor.Value;
		CurrentVisionObstruction = FMath::Max(CurrentVisionObstruction, Descriptor.VisionObstruction);
		CurrentCarriedNoiseRadius = FMath::Max(CurrentCarriedNoiseRadius, Descriptor.CarriedNoiseRadius);
		CurrentRiskPanicPerSecond += Descriptor.PanicPerSecondWhileCarried;
		CurrentLootMovementPenaltyMultiplier = FMath::Min(CurrentLootMovementPenaltyMultiplier, FMath::Clamp(Descriptor.MovementPenaltyMultiplier, 0.1f, 1.0f));
		bCarryingTwoHandedLoot = bCarryingTwoHandedLoot || Descriptor.bRequiresTwoHands;
	}

	const EMCCarryState PreviousState = CarryState;
	if (bDragging)
	{
		CarryState = EMCCarryState::Dragging;
	}
	else if (CurrentCarryWeight >= OverloadedWeightThreshold)
	{
		CarryState = EMCCarryState::Overloaded;
	}
	else if (CurrentCarryWeight >= EncumberedWeightThreshold)
	{
		CarryState = EMCCarryState::Encumbered;
	}
	else
	{
		CarryState = EMCCarryState::Light;
	}

	if (PreviousState != CarryState)
	{
		OnCarryStateChanged.Broadcast(CarryState);
	}
}

FVector UMCCarryComponent::GetDropLocation(int32 DropIndex) const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return FVector::ZeroVector;
	}

	const FVector Forward = Owner->GetActorForwardVector();
	const FVector Right = Owner->GetActorRightVector();
	const float SideOffset = (DropIndex % 3 - 1) * 45.0f;
	return Owner->GetActorLocation() + Forward * 110.0f + Right * SideOffset;
}
