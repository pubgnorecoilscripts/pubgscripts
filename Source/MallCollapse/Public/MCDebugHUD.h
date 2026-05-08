#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MCDebugHUD.generated.h"

UCLASS()
class MALLCOLLAPSE_API AMCDebugHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

protected:
	void DrawLineText(const FString& Text, float& Y, const FColor& Color = FColor::White);
	FString GetInteractionPrompt() const;
};
