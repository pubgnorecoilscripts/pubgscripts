#include "MCInteractionComponent.h"

#include "MCInteractable.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

UMCInteractionComponent::UMCInteractionComponent()
{
	SetIsReplicatedByDefault(true);
}

void UMCInteractionComponent::InteractWithActor(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (Owner && Owner->HasAuthority())
	{
		ProcessInteraction(TargetActor);
	}
	else
	{
		ServerInteractWithActor(TargetActor);
	}
}

void UMCInteractionComponent::TryInteractFromView()
{
	AActor* Owner = GetOwner();
	if (Owner && Owner->HasAuthority())
	{
		ServerInteractFromView_Implementation();
	}
	else
	{
		ServerInteractFromView();
	}
}

void UMCInteractionComponent::ServerInteractWithActor_Implementation(AActor* TargetActor)
{
	ProcessInteraction(TargetActor);
}

void UMCInteractionComponent::ServerInteractFromView_Implementation()
{
	FHitResult HitResult;
	if (FindInteractableFromView(HitResult))
	{
		ProcessInteraction(HitResult.GetActor());
	}
}

bool UMCInteractionComponent::FindInteractableFromView(FHitResult& OutHitResult) const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || !OwnerPawn->GetController() || !GetWorld())
	{
		return false;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	OwnerPawn->GetController()->GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector TraceStart = ViewLocation;
	const FVector TraceEnd = TraceStart + ViewRotation.Vector() * MaxInteractionDistance;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(MallCollapseInteractTrace), false, OwnerPawn);

	const FCollisionShape SweepShape = FCollisionShape::MakeSphere(InteractionTraceRadius);
	const bool bHit = GetWorld()->SweepSingleByChannel(OutHitResult, TraceStart, TraceEnd, FQuat::Identity, InteractionTraceChannel, SweepShape, QueryParams);
	return bHit && IsActorInteractable(OutHitResult.GetActor()) && IsWithinInteractionRange(OutHitResult.GetActor());
}

bool UMCInteractionComponent::IsActorInteractable(AActor* TargetActor) const
{
	return TargetActor && TargetActor->GetClass()->ImplementsInterface(UMCInteractable::StaticClass());
}

bool UMCInteractionComponent::IsWithinInteractionRange(AActor* TargetActor) const
{
	const AActor* Owner = GetOwner();
	if (!Owner || !TargetActor)
	{
		return false;
	}

	return FVector::DistSquared(Owner->GetActorLocation(), TargetActor->GetActorLocation()) <= FMath::Square(MaxInteractionDistance);
}

void UMCInteractionComponent::ProcessInteraction(AActor* TargetActor)
{
	if (!IsActorInteractable(TargetActor) || !IsWithinInteractionRange(TargetActor))
	{
		return;
	}

	APawn* InteractingPawn = Cast<APawn>(GetOwner());
	if (!InteractingPawn)
	{
		return;
	}

	if (IMCInteractable::Execute_CanInteract(TargetActor, InteractingPawn))
	{
		IMCInteractable::Execute_Interact(TargetActor, InteractingPawn);
	}
}
