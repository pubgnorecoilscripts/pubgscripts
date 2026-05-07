#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MallCollapseTypes.h"
#include "MCPlayerController.generated.h"

class AMCPingActor;

UCLASS()
class MALLCOLLAPSE_API AMCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMCPlayerController();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Ping")
	void ServerCreatePing(EMCPingType PingType, FVector_NetQuantize Location, AActor* TargetActor);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ping")
	TSubclassOf<AMCPingActor> PingActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ping")
	float PingLifetimeSeconds = 8.0f;
};
