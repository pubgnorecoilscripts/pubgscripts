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

	UFUNCTION(BlueprintCallable, Category = "Ping")
	void CreatePingFromView(EMCPingType PingType);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Ping")
	void ServerCreatePing(EMCPingType PingType, FVector_NetQuantize Location, AActor* TargetActor);

	UFUNCTION(Server, Reliable, Category = "Ping")
	void ServerCreatePingFromView(EMCPingType PingType);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ping")
	TSubclassOf<AMCPingActor> PingActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ping")
	float PingLifetimeSeconds = 8.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ping")
	float MaxPingDistance = 4000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ping")
	TEnumAsByte<ECollisionChannel> PingTraceChannel = ECC_Visibility;

	bool TracePingFromView(FVector& OutLocation, AActor*& OutTargetActor) const;
};
