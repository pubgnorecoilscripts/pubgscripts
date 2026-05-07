#pragma once

#include "CoreMinimal.h"
#include "MallCollapseTypes.generated.h"

UENUM(BlueprintType)
enum class EMCMatchPhase : uint8
{
	Entry UMETA(DisplayName = "Entry"),
	Instability UMETA(DisplayName = "Instability"),
	CollapseCascade UMETA(DisplayName = "Collapse Cascade"),
	FinalExtraction UMETA(DisplayName = "Final Extraction"),
	MatchEnded UMETA(DisplayName = "Match Ended")
};

UENUM(BlueprintType)
enum class EMCCarryState : uint8
{
	Light UMETA(DisplayName = "Light"),
	Encumbered UMETA(DisplayName = "Encumbered"),
	Overloaded UMETA(DisplayName = "Overloaded"),
	Dragging UMETA(DisplayName = "Dragging")
};

UENUM(BlueprintType)
enum class EMCLootSize : uint8
{
	Small UMETA(DisplayName = "Small"),
	Medium UMETA(DisplayName = "Medium"),
	Large UMETA(DisplayName = "Large"),
	Oversized UMETA(DisplayName = "Oversized")
};

UENUM(BlueprintType)
enum class EMCPanicTier : uint8
{
	Calm UMETA(DisplayName = "Calm"),
	Alert UMETA(DisplayName = "Alert"),
	Stressed UMETA(DisplayName = "Stressed"),
	Panicked UMETA(DisplayName = "Panicked")
};

UENUM(BlueprintType)
enum class EMCHazardType : uint8
{
	Fire UMETA(DisplayName = "Fire"),
	Smoke UMETA(DisplayName = "Smoke"),
	Flood UMETA(DisplayName = "Flood"),
	Electrical UMETA(DisplayName = "Electrical"),
	StructuralCollapse UMETA(DisplayName = "Structural Collapse"),
	SecurityLockdown UMETA(DisplayName = "Security Lockdown")
};

UENUM(BlueprintType)
enum class EMCModuleIntegrityState : uint8
{
	Intact UMETA(DisplayName = "Intact"),
	Damaged UMETA(DisplayName = "Damaged"),
	Hazardous UMETA(DisplayName = "Hazardous"),
	PartiallyCollapsed UMETA(DisplayName = "Partially Collapsed"),
	Blocked UMETA(DisplayName = "Blocked")
};

UENUM(BlueprintType)
enum class EMCExtractionState : uint8
{
	Unknown UMETA(DisplayName = "Unknown"),
	Available UMETA(DisplayName = "Available"),
	TemporarilyBlocked UMETA(DisplayName = "Temporarily Blocked"),
	RequiresPower UMETA(DisplayName = "Requires Power"),
	FailingSoon UMETA(DisplayName = "Failing Soon"),
	Failed UMETA(DisplayName = "Failed")
};

UENUM(BlueprintType)
enum class EMCPingType : uint8
{
	Danger UMETA(DisplayName = "Danger"),
	Loot UMETA(DisplayName = "Loot"),
	Exit UMETA(DisplayName = "Exit"),
	Help UMETA(DisplayName = "Help"),
	SuspiciousPlayer UMETA(DisplayName = "Suspicious Player"),
	FakeExit UMETA(DisplayName = "Fake Exit")
};

UENUM(BlueprintType)
enum class EMCSabotageType : uint8
{
	ReverseEscalator UMETA(DisplayName = "Reverse Escalator"),
	FakeAnnouncement UMETA(DisplayName = "Fake Announcement"),
	LockdownShutter UMETA(DisplayName = "Lockdown Shutter"),
	PowerShutdown UMETA(DisplayName = "Power Shutdown"),
	FloodValve UMETA(DisplayName = "Flood Valve")
};

USTRUCT(BlueprintType)
struct FMCLootDescriptor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	FName LootId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot", meta = (ClampMin = "0"))
	int32 Value = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot", meta = (ClampMin = "0.0"))
	float Weight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	EMCLootSize Size = EMCLootSize::Small;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	bool bRequiresTwoHands = false;
};
