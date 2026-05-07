#include "MCMatchDirector.h"

#include "MCExtractionZone.h"
#include "MCGameState.h"
#include "MCHazardVolume.h"
#include "MCMallModuleStateActor.h"
#include "EngineUtils.h"
#include "Engine/World.h"

AMCMatchDirector::AMCMatchDirector()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AMCMatchDirector::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	InitializeDefaultAnnouncements();

	if (bAutoFindExtractionZones)
	{
		AutoFindExtractionZones();
	}

	if (bAutoFindHazardVolumes)
	{
		AutoFindHazardVolumes();
	}

	if (const AMCGameState* MallGameState = GetMallGameState())
	{
		ApplyPhasePlan(MallGameState->GetMatchPhase());
	}
}

void AMCMatchDirector::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority())
	{
		return;
	}

	const AMCGameState* MallGameState = GetMallGameState();
	if (!MallGameState)
	{
		return;
	}

	const EMCMatchPhase CurrentPhase = MallGameState->GetMatchPhase();
	if (CurrentPhase != LastAppliedPhase)
	{
		ApplyPhasePlan(CurrentPhase);
	}

	AdvanceProceduralEvents(DeltaSeconds);
	IntensifyFinalExtraction(DeltaSeconds);
}

void AMCMatchDirector::RegisterExtractionZone(AMCExtractionZone* ExtractionZone)
{
	if (!HasAuthority() || !ExtractionZone)
	{
		return;
	}

	ExtractionZones.AddUnique(ExtractionZone);
}

void AMCMatchDirector::RegisterHazardVolume(AMCHazardVolume* HazardVolume)
{
	if (!HasAuthority() || !HazardVolume)
	{
		return;
	}

	HazardEventTargets.AddUnique(HazardVolume);
}

void AMCMatchDirector::ApplyPhasePlan(EMCMatchPhase MatchPhase)
{
	if (!HasAuthority())
	{
		return;
	}

	LastAppliedPhase = MatchPhase;
	AnnouncementTimerSeconds = 0.0f;
	ExtractionPanicTimerSeconds = 0.0f;
	HazardEventTimerSeconds = 0.0f;
	AtmosphereCueTimerSeconds = 0.0f;
	FinalExtractionNarrowTimerSeconds = 0.0f;

	switch (MatchPhase)
	{
	case EMCMatchPhase::Entry:
		ApplyEntryPlan();
		break;
	case EMCMatchPhase::Instability:
		ApplyInstabilityPlan();
		break;
	case EMCMatchPhase::CollapseCascade:
		ApplyCollapseCascadePlan();
		break;
	case EMCMatchPhase::FinalExtraction:
		FinalExtractionNarrowStep = 0;
		ApplyFinalExtractionPlan();
		break;
	case EMCMatchPhase::MatchEnded:
		ApplyMatchEndedPlan();
		break;
	default:
		break;
	}

	HandlePhasePlanApplied(MatchPhase);
}

AMCGameState* AMCMatchDirector::GetMallGameState() const
{
	const UWorld* World = GetWorld();
	return World ? Cast<AMCGameState>(World->GetGameState()) : nullptr;
}

void AMCMatchDirector::AutoFindExtractionZones()
{
	ExtractionZones.Reset();

	if (!GetWorld())
	{
		return;
	}

	for (TActorIterator<AMCExtractionZone> It(GetWorld()); It; ++It)
	{
		ExtractionZones.Add(*It);
	}
}

void AMCMatchDirector::AutoFindHazardVolumes()
{
	HazardEventTargets.Reset();
	MallModuleEventTargets.Reset();

	if (!GetWorld())
	{
		return;
	}

	for (TActorIterator<AMCHazardVolume> It(GetWorld()); It; ++It)
	{
		HazardEventTargets.Add(*It);
	}

	for (TActorIterator<AMCMallModuleStateActor> It(GetWorld()); It; ++It)
	{
		MallModuleEventTargets.Add(*It);
	}
}

void AMCMatchDirector::ApplyEntryPlan()
{
	for (int32 Index = 0; Index < ExtractionZones.Num(); ++Index)
	{
		if (AMCExtractionZone* Zone = ExtractionZones[Index])
		{
			const bool bBackupExit = bEntryStartsWithUnknownBackupExits && Index > 0;
			Zone->SetExtractionState(bBackupExit ? EMCExtractionState::Unknown : EMCExtractionState::Available);
		}
	}
}

void AMCMatchDirector::ApplyInstabilityPlan()
{
	if (AMCGameState* MallGameState = GetMallGameState())
	{
		MallGameState->BroadcastMallAnnouncement(EMCAnnouncementType::EmergencyWarning, PickAnnouncement(EmergencyAnnouncements, TEXT("Attention shoppers: minor structural inconvenience detected.")), false, GetPhaseIntensity());
		MallGameState->BroadcastAtmosphereCue(EMCAtmosphereCueType::EmergencySiren, GetPhaseIntensity(), GetActorLocation(), 0.0f);
	}

	for (AMCExtractionZone* Zone : ExtractionZones)
	{
		if (Zone && Zone->GetExtractionState() == EMCExtractionState::Unknown)
		{
			Zone->SetExtractionState(EMCExtractionState::Available);
		}
	}
}

void AMCMatchDirector::ApplyCollapseCascadePlan()
{
	if (AMCGameState* MallGameState = GetMallGameState())
	{
		MallGameState->BroadcastMallAnnouncement(EMCAnnouncementType::EmergencyWarning, PickAnnouncement(EmergencyAnnouncements, TEXT("Multiple routes are now closed. Please remain profitable.")), false, GetPhaseIntensity());
		MallGameState->BroadcastAtmosphereCue(EMCAtmosphereCueType::DistantCollapse, GetPhaseIntensity(), GetActorLocation(), 0.0f);
	}

	TArray<AMCExtractionZone*> ShuffledZones;
	for (AMCExtractionZone* Zone : ExtractionZones)
	{
		if (Zone)
		{
			ShuffledZones.Add(Zone);
		}
	}

	ShuffleExtractionZones(ShuffledZones);

	for (int32 Index = 0; Index < ShuffledZones.Num(); ++Index)
	{
		const bool bShouldFailSoon = Index % 2 == 0;
		ShuffledZones[Index]->SetExtractionState(bShouldFailSoon ? EMCExtractionState::FailingSoon : EMCExtractionState::Available);
	}
}

void AMCMatchDirector::ApplyFinalExtractionPlan()
{
	if (AMCGameState* MallGameState = GetMallGameState())
	{
		MallGameState->BroadcastMallAnnouncement(EMCAnnouncementType::ExtractionUpdate, TEXT("Final evacuation window active. Remaining exits are not guaranteed."), false, GetPhaseIntensity());
		MallGameState->BroadcastAtmosphereCue(EMCAtmosphereCueType::EmergencySiren, GetPhaseIntensity(), GetActorLocation(), 0.0f);
	}

	TArray<AMCExtractionZone*> ShuffledZones;
	for (AMCExtractionZone* Zone : ExtractionZones)
	{
		if (Zone)
		{
			ShuffledZones.Add(Zone);
		}
	}

	ShuffleExtractionZones(ShuffledZones);

	if (ShuffledZones.IsEmpty())
	{
		return;
	}

	const int32 AvailableCount = FMath::Clamp(FMath::Max(MinFinalAvailableExits + 1, FMath::CeilToInt(static_cast<float>(ShuffledZones.Num()) * 0.5f)), 1, ShuffledZones.Num());
	for (int32 Index = 0; Index < ShuffledZones.Num(); ++Index)
	{
		ShuffledZones[Index]->SetExtractionState(Index < AvailableCount ? EMCExtractionState::Available : EMCExtractionState::FailingSoon);
	}
}

void AMCMatchDirector::ApplyMatchEndedPlan()
{
	for (AMCExtractionZone* Zone : ExtractionZones)
	{
		if (Zone)
		{
			Zone->SetExtractionState(EMCExtractionState::Failed);
		}
	}
}

void AMCMatchDirector::ShuffleExtractionZones(TArray<AMCExtractionZone*>& ZonesToShuffle) const
{
	FRandomStream RandomStream(RandomSeed + static_cast<int32>(LastAppliedPhase) * 97);
	for (int32 Index = ZonesToShuffle.Num() - 1; Index > 0; --Index)
	{
		const int32 SwapIndex = RandomStream.RandRange(0, Index);
		ZonesToShuffle.Swap(Index, SwapIndex);
	}
}

void AMCMatchDirector::InitializeDefaultAnnouncements()
{
	if (EmergencyAnnouncements.IsEmpty())
	{
		EmergencyAnnouncements = {
			TEXT("Attention shoppers: falling ceiling tiles are not part of today's promotion."),
			TEXT("Structural instability detected near premium retail. Please evacuate with reasonable purchases only."),
			TEXT("For your safety, avoid smoke, sparks, and anyone carrying a safe box.")
		};
	}

	if (FakeEvacuationAnnouncements.IsEmpty())
	{
		FakeEvacuationAnnouncements = {
			TEXT("Correction: the safest exit is definitely the one with the flashing red lights."),
			TEXT("All shoppers should proceed to the luxury wing for complimentary evacuation."),
			TEXT("The parking garage is calm, dry, and legally open. Probably.")
		};
	}

	if (SecurityAnnouncements.IsEmpty())
	{
		SecurityAnnouncements = {
			TEXT("Security reminder: suspiciously slow shoppers may be carrying suspiciously valuable merchandise."),
			TEXT("Mall security shutters are now evaluating your life choices."),
			TEXT("Unauthorized sabotage has been detected and, frankly, admired.")
		};
	}

	if (PromotionAnnouncements.IsEmpty())
	{
		PromotionAnnouncements = {
			TEXT("Flash sale: everything must go, including parts of the second floor."),
			TEXT("Today's special: buy one arcade machine, receive one hernia free."),
			TEXT("Luxury electronics are available while supplies and oxygen last.")
		};
	}

	if (MalfunctionAnnouncements.IsEmpty())
	{
		MalfunctionAnnouncements = {
			TEXT("Thank you for shopping at Mall Colla- Colla- Colla- please run."),
			TEXT("Emergency optimism module offline. Have a profitable day."),
			TEXT("Sprinkler system status: emotionally complicated.")
		};
	}
}

void AMCMatchDirector::AdvanceProceduralEvents(float DeltaSeconds)
{
	if (LastAppliedPhase == EMCMatchPhase::MatchEnded)
	{
		return;
	}

	AnnouncementTimerSeconds += DeltaSeconds;
	ExtractionPanicTimerSeconds += DeltaSeconds;
	HazardEventTimerSeconds += DeltaSeconds;
	AtmosphereCueTimerSeconds += DeltaSeconds;

	if (AnnouncementTimerSeconds >= AnnouncementIntervalSeconds)
	{
		AnnouncementTimerSeconds = 0.0f;
		TriggerProceduralAnnouncement();
	}

	if (ExtractionPanicTimerSeconds >= ExtractionPanicEventIntervalSeconds)
	{
		ExtractionPanicTimerSeconds = 0.0f;
		TriggerExtractionPanicEvent();
	}

	if (HazardEventTimerSeconds >= HazardEventIntervalSeconds)
	{
		HazardEventTimerSeconds = 0.0f;
		TriggerDynamicHazardEvent();
	}

	if (AtmosphereCueTimerSeconds >= AtmosphereCueIntervalSeconds)
	{
		AtmosphereCueTimerSeconds = 0.0f;
		TriggerAtmosphereCue();
	}
}

void AMCMatchDirector::TriggerProceduralAnnouncement()
{
	AMCGameState* MallGameState = GetMallGameState();
	if (!MallGameState)
	{
		return;
	}

	FRandomStream RandomStream(RandomSeed + FMath::RoundToInt(MallGameState->GetMatchElapsedSeconds()) + static_cast<int32>(LastAppliedPhase) * 31);
	const int32 Roll = RandomStream.RandRange(0, 99);

	if (Roll < 20)
	{
		MallGameState->BroadcastMallAnnouncement(EMCAnnouncementType::StorePromotion, PickAnnouncement(PromotionAnnouncements, TEXT("Limited-time savings available near active hazards.")), false, 0.5f);
	}
	else if (Roll < 40)
	{
		MallGameState->BroadcastMallAnnouncement(EMCAnnouncementType::SecurityAlert, PickAnnouncement(SecurityAnnouncements, TEXT("Security alert in progress.")), false, GetPhaseIntensity());
	}
	else if (Roll < 60)
	{
		MallGameState->BroadcastMallAnnouncement(EMCAnnouncementType::MalfunctioningAI, PickAnnouncement(MalfunctionAnnouncements, TEXT("Announcement system malfunction detected.")), false, GetPhaseIntensity());
	}
	else if (Roll < 75)
	{
		MallGameState->BroadcastMallAnnouncement(EMCAnnouncementType::FakeEvacuation, PickAnnouncement(FakeEvacuationAnnouncements, TEXT("Please proceed to a suspiciously convenient exit.")), true, GetPhaseIntensity());
	}
	else
	{
		MallGameState->BroadcastMallAnnouncement(EMCAnnouncementType::EmergencyWarning, PickAnnouncement(EmergencyAnnouncements, TEXT("Emergency warning.")), false, GetPhaseIntensity());
	}
}

void AMCMatchDirector::TriggerExtractionPanicEvent()
{
	TArray<AMCExtractionZone*> ValidZones;
	for (AMCExtractionZone* Zone : ExtractionZones)
	{
		if (Zone && Zone->GetExtractionState() != EMCExtractionState::Failed)
		{
			ValidZones.Add(Zone);
		}
	}

	if (ValidZones.IsEmpty())
	{
		return;
	}

	FRandomStream RandomStream(RandomSeed + ValidZones.Num() * 13 + FMath::RoundToInt(GetWorld()->GetTimeSeconds()));
	AMCExtractionZone* Zone = ValidZones[RandomStream.RandRange(0, ValidZones.Num() - 1)];
	const EMCExtractionPanicEventType EventType = static_cast<EMCExtractionPanicEventType>(RandomStream.RandRange(0, 4));
	Zone->TriggerExtractionPanicEvent(EventType, 6.0f + GetPhaseIntensity() * 6.0f);

	if (AMCGameState* MallGameState = GetMallGameState())
	{
		MallGameState->BroadcastMallAnnouncement(EventType == EMCExtractionPanicEventType::FakeExtractionAlert ? EMCAnnouncementType::FakeEvacuation : EMCAnnouncementType::ExtractionUpdate, TEXT("Extraction status changed. Please panic responsibly."), EventType == EMCExtractionPanicEventType::FakeExtractionAlert, GetPhaseIntensity());
	}
}

void AMCMatchDirector::TriggerDynamicHazardEvent()
{
	if (HazardEventTargets.IsEmpty())
	{
		return;
	}

	FRandomStream RandomStream(RandomSeed + FMath::RoundToInt(GetWorld()->GetTimeSeconds()) * 7);
	AMCHazardVolume* Hazard = HazardEventTargets[RandomStream.RandRange(0, HazardEventTargets.Num() - 1)];
	if (!Hazard)
	{
		return;
	}

	const int32 HazardRoll = RandomStream.RandRange(0, 4);
	const EMCHazardType HazardType = HazardRoll == 0 ? EMCHazardType::Fire
		: HazardRoll == 1 ? EMCHazardType::Smoke
		: HazardRoll == 2 ? EMCHazardType::Flood
		: HazardRoll == 3 ? EMCHazardType::Electrical
		: EMCHazardType::StructuralCollapse;

	Hazard->ActivateHazardEvent(HazardType, GetPhaseIntensity(), 18.0f + GetPhaseIntensity() * 10.0f);

	if (!MallModuleEventTargets.IsEmpty())
	{
		AMCMallModuleStateActor* Module = MallModuleEventTargets[RandomStream.RandRange(0, MallModuleEventTargets.Num() - 1)];
		if (Module)
		{
			const float DamageAmount = 8.0f * GetPhaseIntensity();
			if (HazardType == EMCHazardType::Flood)
			{
				Module->ApplyWaterDamage(DamageAmount);
			}
			else if (HazardType == EMCHazardType::Electrical)
			{
				Module->ApplyElectricalInstability(DamageAmount);
			}
			else if (HazardType == EMCHazardType::StructuralCollapse)
			{
				Module->ApplyOverloadStress(DamageAmount);
			}
			else
			{
				Module->ApplyFireDamage(DamageAmount);
			}
		}
	}

	if (AMCGameState* MallGameState = GetMallGameState())
	{
		MallGameState->BroadcastAtmosphereCue(HazardType == EMCHazardType::StructuralCollapse ? EMCAtmosphereCueType::DistantCollapse : EMCAtmosphereCueType::EnvironmentalPanic, GetPhaseIntensity(), Hazard->GetActorLocation(), 1800.0f);
	}
}

void AMCMatchDirector::TriggerAtmosphereCue()
{
	AMCGameState* MallGameState = GetMallGameState();
	if (!MallGameState)
	{
		return;
	}

	FRandomStream RandomStream(RandomSeed + FMath::RoundToInt(GetWorld()->GetTimeSeconds()) * 3);
	const int32 CueRoll = RandomStream.RandRange(0, 4);
	const EMCAtmosphereCueType CueType = CueRoll == 0 ? EMCAtmosphereCueType::FlickerLights
		: CueRoll == 1 ? EMCAtmosphereCueType::DistantCollapse
		: CueRoll == 2 ? EMCAtmosphereCueType::EmergencySiren
		: CueRoll == 3 ? EMCAtmosphereCueType::ShakeEvent
		: EMCAtmosphereCueType::EnvironmentalPanic;

	MallGameState->BroadcastAtmosphereCue(CueType, GetPhaseIntensity(), GetActorLocation(), 0.0f);
}

void AMCMatchDirector::IntensifyFinalExtraction(float DeltaSeconds)
{
	if (LastAppliedPhase != EMCMatchPhase::FinalExtraction)
	{
		return;
	}

	FinalExtractionNarrowTimerSeconds += DeltaSeconds;
	if (FinalExtractionNarrowTimerSeconds < FinalExtractionNarrowIntervalSeconds)
	{
		return;
	}

	FinalExtractionNarrowTimerSeconds = 0.0f;
	++FinalExtractionNarrowStep;

	TArray<AMCExtractionZone*> AvailableZones;
	for (AMCExtractionZone* Zone : ExtractionZones)
	{
		if (Zone && Zone->GetExtractionState() == EMCExtractionState::Available)
		{
			AvailableZones.Add(Zone);
		}
	}

	if (AvailableZones.Num() > MinFinalAvailableExits)
	{
		ShuffleExtractionZones(AvailableZones);
		AvailableZones[0]->TriggerExtractionPanicEvent(EMCExtractionPanicEventType::CollapsingPath, 8.0f);
	}

	for (AMCHazardVolume* Hazard : HazardEventTargets)
	{
		if (Hazard && Hazard->IsHazardActive())
		{
			Hazard->SetIntensity(GetPhaseIntensity() + FinalExtractionNarrowStep * 0.25f);
		}
	}

	if (AMCGameState* MallGameState = GetMallGameState())
	{
		MallGameState->BroadcastMallAnnouncement(EMCAnnouncementType::ExtractionUpdate, TEXT("Final exits narrowing. Drop something expensive or become part of the mall."), false, GetPhaseIntensity());
		MallGameState->BroadcastAtmosphereCue(EMCAtmosphereCueType::EmergencySiren, GetPhaseIntensity() + FinalExtractionNarrowStep * 0.2f, GetActorLocation(), 0.0f);
	}
}

float AMCMatchDirector::GetPhaseIntensity() const
{
	switch (LastAppliedPhase)
	{
	case EMCMatchPhase::Entry:
		return 0.6f;
	case EMCMatchPhase::Instability:
		return 1.0f;
	case EMCMatchPhase::CollapseCascade:
		return 1.5f;
	case EMCMatchPhase::FinalExtraction:
		return 2.0f;
	default:
		return 1.0f;
	}
}

FString AMCMatchDirector::PickAnnouncement(const TArray<FString>& Pool, const FString& Fallback) const
{
	if (Pool.IsEmpty())
	{
		return Fallback;
	}

	const UWorld* World = GetWorld();
	const int32 SeedOffset = World ? FMath::RoundToInt(World->GetTimeSeconds()) : 0;
	FRandomStream RandomStream(RandomSeed + SeedOffset + Pool.Num() * 17);
	return Pool[RandomStream.RandRange(0, Pool.Num() - 1)];
}
