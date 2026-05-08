#include "MCLootItem.h"

#include "MCCharacter.h"
#include "MCCarryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

AMCLootItem::AMCLootItem()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(SceneRoot);
	MeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
	MeshComponent->SetIsReplicated(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(CubeMesh.Object);
		MeshComponent->SetWorldScale3D(FVector(0.45f));
	}
}

void AMCLootItem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority() || !bCarried || bExtracted || LootDescriptor.CarriedNoiseRadius <= 0.0f || LootDescriptor.CarriedNoiseInterval <= 0.0f)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!World || World->GetTimeSeconds() < NextNoisePulseTimeSeconds)
	{
		return;
	}

	NextNoisePulseTimeSeconds = World->GetTimeSeconds() + LootDescriptor.CarriedNoiseInterval;
	MulticastCarriedNoisePulse(LootDescriptor.CarriedNoiseRadius);
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
	NextNoisePulseTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() + LootDescriptor.CarriedNoiseInterval : 0.0f;
	SetReplicateMovement(false);

	if (const AMCCharacter* MallCharacter = Cast<AMCCharacter>(NewCarrier))
	{
		AttachToComponent(MallCharacter->GetCarryAttachPoint(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SetActorRelativeLocation(FVector::ZeroVector);
	}
	else
	{
		AttachToActor(NewCarrier, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}

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
	NextNoisePulseTimeSeconds = 0.0f;
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
	NextNoisePulseTimeSeconds = 0.0f;
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

void AMCLootItem::ApplyHighValuePreset(EMCHighValueLootPreset Preset)
{
	if (bCarried || bExtracted)
	{
		return;
	}

	LootDescriptor.bHighValue = true;

	switch (Preset)
	{
	case EMCHighValueLootPreset::GiantTelevision:
		LootDescriptor.LootId = TEXT("giant_television");
		LootDescriptor.DisplayName = NSLOCTEXT("MallCollapse", "GiantTelevision", "Giant Television");
		LootDescriptor.Value = 900;
		LootDescriptor.Weight = 42.0f;
		LootDescriptor.Size = EMCLootSize::Large;
		LootDescriptor.bRequiresTwoHands = true;
		LootDescriptor.MovementPenaltyMultiplier = 0.78f;
		LootDescriptor.VisionObstruction = 0.45f;
		LootDescriptor.CarriedNoiseRadius = 650.0f;
		LootDescriptor.CarriedNoiseInterval = 2.5f;
		LootDescriptor.PanicPerSecondWhileCarried = 0.6f;
		break;
	case EMCHighValueLootPreset::ArcadeMachine:
		LootDescriptor.LootId = TEXT("arcade_machine");
		LootDescriptor.DisplayName = NSLOCTEXT("MallCollapse", "ArcadeMachine", "Arcade Machine");
		LootDescriptor.Value = 1300;
		LootDescriptor.Weight = 58.0f;
		LootDescriptor.Size = EMCLootSize::Oversized;
		LootDescriptor.bRequiresTwoHands = true;
		LootDescriptor.MovementPenaltyMultiplier = 0.62f;
		LootDescriptor.VisionObstruction = 0.32f;
		LootDescriptor.CarriedNoiseRadius = 900.0f;
		LootDescriptor.CarriedNoiseInterval = 1.8f;
		LootDescriptor.PanicPerSecondWhileCarried = 1.0f;
		break;
	case EMCHighValueLootPreset::JewelryCase:
		LootDescriptor.LootId = TEXT("jewelry_case");
		LootDescriptor.DisplayName = NSLOCTEXT("MallCollapse", "JewelryCase", "Jewelry Case");
		LootDescriptor.Value = 1800;
		LootDescriptor.Weight = 22.0f;
		LootDescriptor.Size = EMCLootSize::Medium;
		LootDescriptor.bRequiresTwoHands = true;
		LootDescriptor.MovementPenaltyMultiplier = 0.9f;
		LootDescriptor.VisionObstruction = 0.2f;
		LootDescriptor.CarriedNoiseRadius = 500.0f;
		LootDescriptor.CarriedNoiseInterval = 3.5f;
		LootDescriptor.PanicPerSecondWhileCarried = 0.4f;
		break;
	case EMCHighValueLootPreset::LuxuryElectronics:
		LootDescriptor.LootId = TEXT("luxury_electronics");
		LootDescriptor.DisplayName = NSLOCTEXT("MallCollapse", "LuxuryElectronics", "Luxury Electronics");
		LootDescriptor.Value = 1100;
		LootDescriptor.Weight = 16.0f;
		LootDescriptor.Size = EMCLootSize::Medium;
		LootDescriptor.bRequiresTwoHands = false;
		LootDescriptor.MovementPenaltyMultiplier = 0.96f;
		LootDescriptor.VisionObstruction = 0.08f;
		LootDescriptor.CarriedNoiseRadius = 350.0f;
		LootDescriptor.CarriedNoiseInterval = 4.0f;
		LootDescriptor.PanicPerSecondWhileCarried = 0.25f;
		break;
	case EMCHighValueLootPreset::SafeBox:
		LootDescriptor.LootId = TEXT("safe_box");
		LootDescriptor.DisplayName = NSLOCTEXT("MallCollapse", "SafeBox", "Safe Box");
		LootDescriptor.Value = 2200;
		LootDescriptor.Weight = 66.0f;
		LootDescriptor.Size = EMCLootSize::Oversized;
		LootDescriptor.bRequiresTwoHands = true;
		LootDescriptor.MovementPenaltyMultiplier = 0.55f;
		LootDescriptor.VisionObstruction = 0.25f;
		LootDescriptor.CarriedNoiseRadius = 750.0f;
		LootDescriptor.CarriedNoiseInterval = 2.0f;
		LootDescriptor.PanicPerSecondWhileCarried = 1.2f;
		break;
	default:
		break;
	}
}

void AMCLootItem::OnRep_Carried()
{
	ApplyCarryPresentation();
}

void AMCLootItem::MulticastCarriedNoisePulse_Implementation(float NoiseRadius)
{
	HandleCarriedNoisePulse(NoiseRadius);
}

void AMCLootItem::ApplyCarryPresentation()
{
	const bool bCollisionEnabled = !bCarried && !bExtracted;
	SetActorEnableCollision(bCollisionEnabled);

	if (MeshComponent)
	{
		MeshComponent->SetSimulatePhysics(false);
		MeshComponent->SetCollisionEnabled(bCollisionEnabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);

		FVector VisualScale(0.45f);
		if (LootDescriptor.Size == EMCLootSize::Medium)
		{
			VisualScale = FVector(0.7f, 0.45f, 0.45f);
		}
		else if (LootDescriptor.Size == EMCLootSize::Large)
		{
			VisualScale = FVector(1.0f, 0.18f, 0.65f);
		}
		else if (LootDescriptor.Size == EMCLootSize::Oversized)
		{
			VisualScale = FVector(1.1f, 0.55f, 0.8f);
		}
		MeshComponent->SetRelativeScale3D(VisualScale);
	}
}
