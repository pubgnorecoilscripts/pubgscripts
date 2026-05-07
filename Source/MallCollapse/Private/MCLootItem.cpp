#include "MCLootItem.h"

#include "MCCarryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

AMCLootItem::AMCLootItem()
{
	bReplicates = true;
	SetReplicateMovement(true);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(SceneRoot);
	MeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
	MeshComponent->SetIsReplicated(true);
}

void AMCLootItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMCLootItem, LootDescriptor);
	DOREPLIFETIME(AMCLootItem, bCarried);
	DOREPLIFETIME(AMCLootItem, bExtracted);
	DOREPLIFETIME(AMCLootItem, Carrier);
}

bool AMCLootItem::CanBePickedUp() const
{
	return !bCarried && !bExtracted;
}

void AMCLootItem::MarkCarried(AActor* NewCarrier)
{
	if (!HasAuthority() || !NewCarrier || !CanBePickedUp())
	{
		return;
	}

	Carrier = NewCarrier;
	bCarried = true;
	SetReplicateMovement(false);
	AttachToActor(NewCarrier, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	ApplyCarryPresentation();
}

void AMCLootItem::MarkDropped(const FVector& DropLocation)
{
	if (!HasAuthority() || bExtracted)
	{
		return;
	}

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Carrier = nullptr;
	bCarried = false;
	SetActorLocation(DropLocation);
	SetReplicateMovement(true);
	ApplyCarryPresentation();
}

void AMCLootItem::MarkExtracted()
{
	if (!HasAuthority() || bExtracted)
	{
		return;
	}

	bExtracted = true;
	bCarried = false;
	Carrier = nullptr;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetLifeSpan(0.1f);
}

FMCLootDescriptor AMCLootItem::GetLootDescriptor() const
{
	return LootDescriptor;
}

bool AMCLootItem::CanInteract_Implementation(APawn* InteractingPawn) const
{
	return InteractingPawn && CanBePickedUp();
}

void AMCLootItem::Interact_Implementation(APawn* InteractingPawn)
{
	if (!HasAuthority() || !InteractingPawn || !CanBePickedUp())
	{
		return;
	}

	if (UMCCarryComponent* CarryComponent = InteractingPawn->FindComponentByClass<UMCCarryComponent>())
	{
		CarryComponent->AddLoot(this);
	}
}

FText AMCLootItem::GetInteractionText_Implementation() const
{
	if (LootDescriptor.DisplayName.IsEmpty())
	{
		return NSLOCTEXT("MallCollapse", "TakeLoot", "Take Loot");
	}

	return FText::Format(NSLOCTEXT("MallCollapse", "TakeLootFormat", "Take {0}"), LootDescriptor.DisplayName);
}

void AMCLootItem::OnRep_Carried()
{
	ApplyCarryPresentation();
}

void AMCLootItem::ApplyCarryPresentation()
{
	const bool bCollisionEnabled = !bCarried && !bExtracted;
	SetActorEnableCollision(bCollisionEnabled);

	if (MeshComponent)
	{
		MeshComponent->SetSimulatePhysics(false);
		MeshComponent->SetCollisionEnabled(bCollisionEnabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	}
}
