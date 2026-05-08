#include "MCCharacter.h"

#include "MCCarryComponent.h"
#include "MCInteractionComponent.h"
#include "MCPlayerController.h"
#include "MCPanicComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"

AMCCharacter::AMCCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 420.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	CarryAttachPoint = CreateDefaultSubobject<USceneComponent>(TEXT("CarryAttachPoint"));
	CarryAttachPoint->SetupAttachment(RootComponent);
	CarryAttachPoint->SetRelativeLocation(FVector(115.0f, 0.0f, 45.0f));

	CarryComponent = CreateDefaultSubobject<UMCCarryComponent>(TEXT("CarryComponent"));
	PanicComponent = CreateDefaultSubobject<UMCPanicComponent>(TEXT("PanicComponent"));
	InteractionComponent = CreateDefaultSubobject<UMCInteractionComponent>(TEXT("InteractionComponent"));

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
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
	if (EnhancedInputComponent && MoveAction)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMCCharacter::HandleMoveInput);
	}

	if (EnhancedInputComponent && LookAction)
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMCCharacter::HandleLookInput);
	}

	if (EnhancedInputComponent && InteractAction)
	{
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AMCCharacter::HandleInteractInput);
	}

	if (EnhancedInputComponent && DropLootAction)
	{
		EnhancedInputComponent->BindAction(DropLootAction, ETriggerEvent::Started, this, &AMCCharacter::HandleDropLootInput);
	}

	if (EnhancedInputComponent && PingAction)
	{
		EnhancedInputComponent->BindAction(PingAction, ETriggerEvent::Started, this, &AMCCharacter::HandlePingInput);
	}

	// Legacy bindings make the source-only prototype playable before input assets exist.
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMCCharacter::HandleMoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMCCharacter::HandleMoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMCCharacter::HandleTurn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMCCharacter::HandleLookUp);
	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AMCCharacter::HandleInteractInput);
	PlayerInputComponent->BindAction(TEXT("DropLoot"), IE_Pressed, this, &AMCCharacter::HandleDropLootInput);
	PlayerInputComponent->BindAction(TEXT("Ping"), IE_Pressed, this, &AMCCharacter::HandlePingInput);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);
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
	ApplyMovementVector(Value.Get<FVector2D>());
}

void AMCCharacter::ApplyMovementVector(const FVector2D& MovementVector)
{
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

void AMCCharacter::HandleMoveForward(float Value)
{
	if (FMath::IsNearlyZero(Value))
	{
		return;
	}

	ApplyMovementVector(FVector2D(0.0f, Value));
}

void AMCCharacter::HandleMoveRight(float Value)
{
	if (FMath::IsNearlyZero(Value))
	{
		return;
	}

	ApplyMovementVector(FVector2D(Value, 0.0f));
}

void AMCCharacter::HandleTurn(float Value)
{
	AddControllerYawInput(Value);
}

void AMCCharacter::HandleLookUp(float Value)
{
	AddControllerPitchInput(Value);
}
