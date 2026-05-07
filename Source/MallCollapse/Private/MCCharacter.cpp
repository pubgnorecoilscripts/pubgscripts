#include "MCCharacter.h"

#include "MCCarryComponent.h"
#include "MCInteractionComponent.h"
#include "MCPlayerController.h"
#include "MCPanicComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"

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

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				if (DefaultInputMappingContext)
				{
					InputSubsystem->AddMappingContext(DefaultInputMappingContext, DefaultInputMappingPriority);
				}
			}
		}
	}
}

void AMCCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ApplyMallMovementModifiers();
}

void AMCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		return;
	}

	if (MoveAction)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMCCharacter::HandleMoveInput);
	}

	if (LookAction)
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMCCharacter::HandleLookInput);
	}

	if (InteractAction)
	{
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AMCCharacter::HandleInteractInput);
	}

	if (DropLootAction)
	{
		EnhancedInputComponent->BindAction(DropLootAction, ETriggerEvent::Started, this, &AMCCharacter::HandleDropLootInput);
	}

	if (PingAction)
	{
		EnhancedInputComponent->BindAction(PingAction, ETriggerEvent::Started, this, &AMCCharacter::HandlePingInput);
	}
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

void AMCCharacter::HandleMoveInput(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (!Controller || MovementVector.IsNearlyZero())
	{
		return;
	}

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AMCCharacter::HandleLookInput(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();
	if (LookVector.IsNearlyZero())
	{
		return;
	}

	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

void AMCCharacter::HandleInteractInput()
{
	if (InteractionComponent)
	{
		InteractionComponent->TryInteractFromView();
	}
}

void AMCCharacter::HandleDropLootInput()
{
	if (!CarryComponent)
	{
		return;
	}

	if (HasAuthority())
	{
		CarryComponent->DropAllLoot();
	}
	else
	{
		CarryComponent->ServerDropAllLoot();
	}
}

void AMCCharacter::HandlePingInput()
{
	if (AMCPlayerController* MallPlayerController = Cast<AMCPlayerController>(Controller))
	{
		MallPlayerController->CreatePingFromView(EMCPingType::Danger);
	}
}
