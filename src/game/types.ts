export type RunPhase = "setup" | "running" | "collapsed" | "cashedOut";

export type Sentiment = "bullish" | "bearish" | "unhinged" | "regulatory" | "bot";

export type CoinConfig = {
  ticker: string;
  narrative: string;
  seed: number;
};

export type CoinState = {
  ticker: string;
  narrative: string;
  price: number;
  liquidity: number;
  marketCap: number;
  hype: number;
  trust: number;
  heat: number;
  attention: number;
  volatility: number;
};

export type InfluencerStatus = "loyal" | "bored" | "rogue" | "cancelled";

export type Influencer = {
  id: string;
  handle: string;
  archetype: string;
  reach: number;
  credibility: number;
  alignment: number;
  volatility: number;
  status: InfluencerStatus;
};

export type ActionId = "botBurst" | "sponsorInfluencer" | "leakRoadmap" | "manufactureOutrage" | "liquidityMirage";

export type ActionDefinition = {
  id: ActionId;
  label: string;
  command: string;
  description: string;
  cost: number;
  cooldown: number;
  effects: {
    hype: number;
    attention: number;
    liquidity: number;
    heat: number;
    trust: number;
    volatility: number;
  };
};

export type CrisisId = "regulatorPing" | "rogueInfluencer" | "whaleDump" | "fakeHack" | "outrageBackfire";

export type CrisisChoice = {
  id: string;
  label: string;
  description: string;
  effects: Partial<Pick<CoinState, "hype" | "trust" | "heat" | "attention" | "liquidity" | "volatility">> & {
    cash?: number;
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
};

export type SocialPost = {
  id: string;
  tick: number;
  author: string;
  body: string;
  sentiment: Sentiment;
  impact: number;
};

export type MarketPoint = {
  tick: number;
  price: number;
  liquidity: number;
  hype: number;
  trust: number;
};

export type RunStats = {
  peakMarketCap: number;
  peakHype: number;
  crisesSurvived: number;
  actionsTaken: number;
  rugPulls: number;
};

export type GameState = {
  phase: RunPhase;
  seed: number;
  rngState: number;
  tick: number;
  cash: number;
  coin: CoinState | null;
  influencers: Influencer[];
  feed: SocialPost[];
  crises: Crisis[];
  market: MarketPoint[];
  cooldowns: Partial<Record<ActionId, number>>;
  stats: RunStats;
  terminal: string[];
};

export type TickResult = {
  state: GameState;
  posts: SocialPost[];
  crisis?: Crisis;
};
