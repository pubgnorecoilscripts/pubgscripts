#include "MCCharacter.h"

#include "MCCarryComponent.h"
#include "MCInteractionComponent.h"
#include "MCPanicComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AMCCharacter::AMCCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CarryComponent = CreateDefaultSubobject<UMCCarryComponent>(TEXT("CarryComponent"));
	PanicComponent = CreateDefaultSubobject<UMCPanicComponent>(TEXT("PanicComponent"));
	InteractionComponent = CreateDefaultSubobject<UMCInteractionComponent>(TEXT("InteractionComponent"));
}

void AMCCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (const UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		BaseWalkSpeed = MovementComponent->MaxWalkSpeed;
	}
}

void AMCCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ApplyMallMovementModifiers();
}

void AMCCharacter::ApplyMallMovementModifiers()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent)
	{
		return;
	}

	const float CarryMultiplier = CarryComponent ? CarryComponent->GetMovementSpeedMultiplier() : 1.0f;
	const float PanicMultiplier = PanicComponent ? PanicComponent->GetMovementStabilityMultiplier() : 1.0f;
	MovementComponent->MaxWalkSpeed = BaseWalkSpeed * CarryMultiplier * PanicMultiplier;
}
