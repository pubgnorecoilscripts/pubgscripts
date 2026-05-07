#include "MCInteractionComponent.h"

#include "MCInteractable.h"
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
		ServerInteractWithActor_Implementation(TargetActor);
	}
	else
	{
		ServerInteractWithActor(TargetActor);
	}
}

void UMCInteractionComponent::ServerInteractWithActor_Implementation(AActor* TargetActor)
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
