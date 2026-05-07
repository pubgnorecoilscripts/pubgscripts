#include "MCCarryComponent.h"

#include "MCLootItem.h"
#include "Net/UnrealNetwork.h"

UMCCarryComponent::UMCCarryComponent()
{
	SetIsReplicatedByDefault(true);
}

void UMCCarryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMCCarryComponent, CarryState);
	DOREPLIFETIME(UMCCarryComponent, CurrentCarryWeight);
	DOREPLIFETIME(UMCCarryComponent, CurrentCarryValue);
	DOREPLIFETIME(UMCCarryComponent, bDragging);
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
		return 1.0f;
	case EMCCarryState::Encumbered:
		return 0.86f;
	case EMCCarryState::Overloaded:
		return 0.64f;
	case EMCCarryState::Dragging:
		return 0.45f;
	default:
		return 1.0f;
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

	for (const AMCLootItem* LootItem : CarriedItems)
	{
		if (!LootItem)
		{
			continue;
		}

		const FMCLootDescriptor Descriptor = LootItem->GetLootDescriptor();
		CurrentCarryWeight += Descriptor.Weight;
		CurrentCarryValue += Descriptor.Value;
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
