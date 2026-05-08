#pragma once

#include "CoreMinimal.h"
#include "Engine/NetSerialization.h"
#include "MallCollapseTypes.generated.h"

class APlayerState;

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
enum class EMCHighValueLootPreset : uint8
{
	GiantTelevision UMETA(DisplayName = "Giant Television"),
	ArcadeMachine UMETA(DisplayName = "Arcade Machine"),
	JewelryCase UMETA(DisplayName = "Jewelry Case"),
	LuxuryElectronics UMETA(DisplayName = "Luxury Electronics"),
	SafeBox UMETA(DisplayName = "Safe Box")
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
enum class EMCExtractionPanicEventType : uint8
{
	ElevatorDelay UMETA(DisplayName = "Elevator Delay"),
	FakeExtractionAlert UMETA(DisplayName = "Fake Extraction Alert"),
	EmergencyReroute UMETA(DisplayName = "Emergency Reroute"),
	CollapsingPath UMETA(DisplayName = "Collapsing Path"),
	LastSecondCountdown UMETA(DisplayName = "Last Second Countdown")
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
	FloodValve UMETA(DisplayName = "Flood Valve"),
	SecurityShutterLockdown UMETA(DisplayName = "Security Shutter Lockdown"),
	BreakerBlackout UMETA(DisplayName = "Breaker Blackout"),
	EscalatorShutdown UMETA(DisplayName = "Escalator Shutdown"),
	SprinklerFlooding UMETA(DisplayName = "Sprinkler Flooding"),
	FalseEmergencyAlarm UMETA(DisplayName = "False Emergency Alarm")
};

UENUM(BlueprintType)
enum class EMCAnnouncementType : uint8
{
	EmergencyWarning UMETA(DisplayName = "Emergency Warning"),
	FakeEvacuation UMETA(DisplayName = "Fake Evacuation"),
	SecurityAlert UMETA(DisplayName = "Security Alert"),
	StorePromotion UMETA(DisplayName = "Store Promotion"),
	MalfunctioningAI UMETA(DisplayName = "Malfunctioning AI"),
	ExtractionUpdate UMETA(DisplayName = "Extraction Update")
};

UENUM(BlueprintType)
enum class EMCAtmosphereCueType : uint8
{
	FlickerLights UMETA(DisplayName = "Flicker Lights"),
	DistantCollapse UMETA(DisplayName = "Distant Collapse"),
	EmergencySiren UMETA(DisplayName = "Emergency Siren"),
	ShakeEvent UMETA(DisplayName = "Shake Event"),
	EnvironmentalPanic UMETA(DisplayName = "Environmental Panic"),
	Blackout UMETA(DisplayName = "Blackout")
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot|Risk")
	bool bHighValue = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot|Risk", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MovementPenaltyMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot|Risk", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float VisionObstruction = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot|Risk", meta = (ClampMin = "0.0"))
	float CarriedNoiseRadius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot|Risk", meta = (ClampMin = "0.0"))
	float CarriedNoiseInterval = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot|Risk", meta = (ClampMin = "0.0"))
	float PanicPerSecondWhileCarried = 0.0f;
};

USTRUCT(BlueprintType)
struct FMCMallAnnouncement
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Announcement")
	int32 SequenceId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Announcement")
	EMCAnnouncementType AnnouncementType = EMCAnnouncementType::EmergencyWarning;

	UPROPERTY(BlueprintReadOnly, Category = "Announcement")
	FString Message;

	UPROPERTY(BlueprintReadOnly, Category = "Announcement")
	bool bFake = false;

	UPROPERTY(BlueprintReadOnly, Category = "Announcement")
	float Intensity = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Announcement")
	float MatchTimeSeconds = 0.0f;
};

USTRUCT(BlueprintType)
struct FMCAtmosphereCue
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Atmosphere")
	int32 SequenceId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Atmosphere")
	EMCAtmosphereCueType CueType = EMCAtmosphereCueType::EnvironmentalPanic;

	UPROPERTY(BlueprintReadOnly, Category = "Atmosphere")
	float Intensity = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Atmosphere")
	float Radius = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Atmosphere")
	FVector_NetQuantize Location = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Atmosphere")
	float MatchTimeSeconds = 0.0f;
};

USTRUCT(BlueprintType)
struct FMCPlayerExtractionResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Extraction")
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Extraction")
	FString PlayerName;

	UPROPERTY(BlueprintReadOnly, Category = "Extraction")
	int32 ExtractedValue = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Extraction")
	float MatchTimeSeconds = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Extraction")
	EMCMatchPhase MatchPhase = EMCMatchPhase::Entry;
};
