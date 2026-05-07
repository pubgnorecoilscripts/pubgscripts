#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MallCollapseTypes.h"
#include "MCPingActor.generated.h"

class AController;

UCLASS(Blueprintable)
class MALLCOLLAPSE_API AMCPingActor : public AActor
{
	GENERATED_BODY()

public:
	AMCPingActor();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Ping")
	void InitializePing(EMCPingType NewPingType, AController* NewOwningController, AActor* NewTargetActor, float NewLifetimeSeconds);

	UFUNCTION(BlueprintPure, Category = "Ping")
	EMCPingType GetPingType() const { return PingType; }

	UFUNCTION(BlueprintPure, Category = "Ping")
	AController* GetOwningController() const { return OwningController; }

protected:
	UPROPERTY(ReplicatedUsing = OnRep_PingData, BlueprintReadOnly, Category = "Ping")
	EMCPingType PingType = EMCPingType::Danger;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ping")
	TObjectPtr<AController> OwningController;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ping")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ping")
	float DefaultLifetimeSeconds = 8.0f;

	UFUNCTION()
	void OnRep_PingData();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ping")
	void HandlePingDataChanged();
};
