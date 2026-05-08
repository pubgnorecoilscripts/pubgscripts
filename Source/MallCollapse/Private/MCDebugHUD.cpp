#include "MCDebugHUD.h"

#include "MCCarryComponent.h"
#include "MCExtractionZone.h"
#include "MCGameState.h"
#include "MCInteractable.h"
#include "MCInteractionComponent.h"
#include "MCPanicComponent.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"

void AMCDebugHUD::DrawHUD()
{
	Super::DrawHUD();

	float Y = 40.0f;
	const AMCGameState* MallGameState = GetWorld() ? Cast<AMCGameState>(GetWorld()->GetGameState()) : nullptr;
	const APawn* Pawn = PlayerOwner ? PlayerOwner->GetPawn() : nullptr;
	const UMCCarryComponent* CarryComponent = Pawn ? Pawn->FindComponentByClass<UMCCarryComponent>() : nullptr;
	const UMCPanicComponent* PanicComponent = Pawn ? Pawn->FindComponentByClass<UMCPanicComponent>() : nullptr;

	DrawLineText(TEXT("MALL COLLAPSE FIRST PLAYABLE DEBUG"), Y, FColor::Yellow);
	DrawLineText(FString::Printf(TEXT("Phase: %s"), MallGameState ? *UEnum::GetValueAsString(MallGameState->GetMatchPhase()) : TEXT("No GameState")), Y);
	DrawLineText(FString::Printf(TEXT("Carried Value: %d"), CarryComponent ? CarryComponent->GetCurrentCarryValue() : 0), Y);
	DrawLineText(FString::Printf(TEXT("Carry State: %s"), CarryComponent ? *UEnum::GetValueAsString(CarryComponent->GetCarryState()) : TEXT("None")), Y);
	DrawLineText(FString::Printf(TEXT("Panic: %.0f"), PanicComponent ? PanicComponent->GetPanic() : 0.0f), Y);

	FString ExtractionText = TEXT("Extraction: none");
	if (GetWorld())
	{
		for (TActorIterator<AMCExtractionZone> It(GetWorld()); It; ++It)
		{
			ExtractionText = FString::Printf(TEXT("Extraction: %s"), *UEnum::GetValueAsString(It->GetExtractionState()));
			break;
		}
	}
	DrawLineText(ExtractionText, Y);

	const FString Prompt = GetInteractionPrompt();
	if (!Prompt.IsEmpty())
	{
		DrawLineText(FString::Printf(TEXT("[E] %s"), *Prompt), Y, FColor::Green);
	}
	else
	{
		DrawLineText(TEXT("[E] Interact | [Q] Drop Loot | Middle Mouse Ping"), Y, FColor::Silver);
	}
}

void AMCDebugHUD::DrawLineText(const FString& Text, float& Y, const FColor& Color)
{
	if (!Canvas)
	{
		return;
	}

	Canvas->SetDrawColor(Color);
	Canvas->DrawText(GEngine ? GEngine->GetSmallFont() : nullptr, Text, 40.0f, Y, 1.0f, 1.0f, FFontRenderInfo());
	Y += 22.0f;
}

FString AMCDebugHUD::GetInteractionPrompt() const
{
	const APawn* Pawn = PlayerOwner ? PlayerOwner->GetPawn() : nullptr;
	const UMCInteractionComponent* InteractionComponent = Pawn ? Pawn->FindComponentByClass<UMCInteractionComponent>() : nullptr;
	if (!InteractionComponent)
	{
		return FString();
	}

	FHitResult HitResult;
	if (!InteractionComponent->FindInteractableFromView(HitResult))
	{
		return FString();
	}

	AActor* HitActor = HitResult.GetActor();
	if (!HitActor || !HitActor->GetClass()->ImplementsInterface(UMCInteractable::StaticClass()))
	{
		return FString();
	}

	return IMCInteractable::Execute_GetInteractionText(HitActor).ToString();
}
