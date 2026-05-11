import type { FactionState } from "./factions";
import type { FrenzyState } from "./frenzy";
import type { MutationId } from "./mutations";

export type Sentiment = "bullish" | "bearish" | "unhinged" | "regulatory" | "bot";

export type MarketSpeed = "focus" | "normal" | "frenzy" | "meltdown";

export type SeasonId =
  | "aiBubble"
  | "antiTechPanic"
  | "memeRenaissance"
  | "doomEconomy"
  | "regulatoryPurge"
  | "conspiracyBoom"
  | "quantumMania";

export type Season = {
  id: SeasonId;
  label: string;
  description: string;
  ticksRemaining: number;
  effects: {
    volatilityMod: number;
    hypeDecayMod: number;
    trustFloor: number;
    crisisRateMod: number;
    factionPowerMods: Partial<Record<string, number>>;
    narrativeBonus: string[];
  };
};

export type OperatorStats = {
  totalInfluence: number;
  peakInfluence: number;
  narrativesLaunched: number;
  narrativesCollapsed: number;
  totalLiquidityExtracted: number;
  ecosystemScars: number;
  factionsCorrupted: number;
  influencersBetraged: number;
  crisesExploited: number;
  frenziesTriggered: number;
  dangerousPlays: number;
  crisisFreezes: number;
  seasonsSurvived: number;
};

export type EcosystemScar = {
  id: string;
  tick: number;
  type: "rugPull" | "collapse" | "regulatorSeizure" | "influencerMartyr" | "factionWar" | "conspiracyBloom";
  title: string;
  description: string;
  effects: Partial<CoinState>;
  factionEffects: Partial<Record<string, number>>;
  permanent: boolean;
};

export type NarrativeState = "active" | "collapsing" | "dead" | "legendary";

export type Narrative = {
  id: string;
  ticker: string;
  narrative: string;
  state: NarrativeState;
  launchedAt: number;
  collapsedAt?: number;
  coin: CoinState;
  cooldowns: Partial<Record<ActionId, number>>;
  momentumChains: MomentumChain[];
  crises: Crisis[];
  delayedRisks: DelayedRisk[];
  frenzy: FrenzyState;
  crisisFreezeCooldown: number;
  influenceGenerated: number;
};

export type CoinState = {
  ticker: string;
  narrative: string;
  price: number;
  liquidity: number;
  marketCap: number;
  volume: number;
  priceVelocity: number;
  momentum: number;
  hype: number;
  trust: number;
  heat: number;
  attention: number;
  volatility: number;
  narrativeStability: number;
};

export type InfluencerTrait =
  | "opportunist"
  | "cultLeader"
  | "paranoid"
  | "accelerationist"
  | "chaosAddict"
  | "regulatorBait"
  | "conspiracyTheorist"
  | "narcissist";

export type InfluencerStatus = "loyal" | "bored" | "rogue" | "cancelled" | "ascended" | "martyred";

export type Influencer = {
  id: string;
  handle: string;
  archetype: string;
  reach: number;
  credibility: number;
  alignment: number;
  volatility: number;
  status: InfluencerStatus;
  trait: InfluencerTrait;
  audience: number;
  opinions: number;
  autonomy: number;
  lastActedAt: number;
  narrativeId?: string;
};

export type ActionId =
  | "botBurst"
  | "sponsorInfluencer"
  | "leakRoadmap"
  | "manufactureOutrage"
  | "liquidityMirage"
  | "fakePartnership"
  | "deployAiProphet"
  | "syntheticOutrage"
  | "leakInternalDocs"
  | "simulatedHack";

export type ActionDefinition = {
  id: ActionId;
  label: string;
  command: string;
  description: string;
  cost: number;
  cooldown: number;
  dangerous?: boolean;
  delayedRisk?: {
    chance: number;
    minDelay: number;
    maxDelay: number;
    crisisType?: CrisisId;
    description: string;
  };
  effects: {
    hype: number;
    attention: number;
    liquidity: number;
    heat: number;
    trust: number;
    volatility: number;
    narrativeStability: number;
  };
  chain: {
    label: string;
    duration: number;
    hypeVelocity: number;
    liquidityMultiplier: number;
    heatAcceleration: number;
    volatilityAcceleration: number;
    synergy: Partial<Record<ActionId, number>>;
  };
};

export type CrisisId = "regulatorPing" | "rogueInfluencer" | "whaleDump" | "fakeHack" | "outrageBackfire";

export type CrisisChoice = {
  id: string;
  label: string;
  description: string;
  effects: Partial<
    Pick<CoinState, "hype" | "trust" | "heat" | "attention" | "liquidity" | "volatility" | "narrativeStability" | "momentum">
  > & {
    cash?: number;
    influence?: number;
  };
};

export type Crisis = {
  id: string;
  type: CrisisId;
  title: string;
  body: string;
  severity: number;
  expiresAt: number;
  choices: CrisisChoice[];
  mutationSource?: MutationId;
  mutationGeneration?: number;
  frozen?: boolean;
  narrativeId: string;
};

export type SocialPost = {
  id: string;
  tick: number;
  author: string;
  body: string;
  sentiment: Sentiment;
  impact: number;
  factionSource?: string;
  effects: Partial<Pick<CoinState, "hype" | "trust" | "heat" | "attention" | "volatility" | "narrativeStability" | "momentum">>;
};

export type MomentumChain = {
  id: string;
  source: ActionId;
  label: string;
  startedAt: number;
  expiresAt: number;
  intensity: number;
  hypeVelocity: number;
  liquidityMultiplier: number;
  heatAcceleration: number;
  volatilityAcceleration: number;
  synergy: Partial<Record<ActionId, number>>;
};

export type DelayedRisk = {
  id: string;
  sourceAction: ActionId;
  triggerAt: number;
  crisisType?: CrisisId;
  description: string;
};

export type MarketEvent = {
  tick: number;
  type: "whale" | "panic" | "euphoria" | "frenzy" | "crash" | "mutation" | "scar";
  intensity: number;
};

export type MarketPoint = {
  tick: number;
  price: number;
  liquidity: number;
  volume: number;
  priceVelocity: number;
  momentum: number;
  hype: number;
  trust: number;
  narrativeStability: number;
  event?: MarketEvent;
};

export type WorldState = {
  seed: number;
  rngState: number;
  tick: number;
  lastTickAt: number;
  cash: number;
  influence: number;
  reach: number;
  notoriety: number;
  activeNarrative: Narrative | null;
  deadNarratives: { ticker: string; influence: number; cause: string }[];
  influencers: Influencer[];
  feed: SocialPost[];
  factions: FactionState[];
  market: MarketPoint[];
  scars: EcosystemScar[];
  season: Season;
  stats: OperatorStats;
  terminal: string[];
  speed: MarketSpeed;
};
