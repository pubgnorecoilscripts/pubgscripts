import type { CrisisId } from "./types";
import type { Rng } from "./rng";

export type MutationId =
  | "dataLeak"
  | "identityExposure"
  | "regulatorInvestigation"
  | "conspiracyMovement"
  | "communityPanic"
  | "bankRun"
  | "confidenceCollapse"
  | "copycatSelloff"
  | "mediaFrenzy"
  | "cultFormation"
  | "aiSchism"
  | "rivalCommunity"
  | "publicCancellation"
  | "classAction"
  | "memeMartyrdom";

export type MutationDef = {
  id: MutationId;
  title: string;
  body: string;
  baseSeverity: number;
  effects: {
    hype?: number;
    trust?: number;
    heat?: number;
    attention?: number;
    liquidity?: number;
    volatility?: number;
    narrativeStability?: number;
    momentum?: number;
  };
  spawnCondition?: (context: MutationContext) => boolean;
  chainInto?: MutationId[];
};

export type MutationContext = {
  parentSeverity: number;
  heat: number;
  trust: number;
  hype: number;
  narrativeStability: number;
  activeCrisisCount: number;
  tick: number;
};

export type MutationTree = {
  source: CrisisId;
  branches: MutationId[];
};

export const mutationTrees: MutationTree[] = [
  {
    source: "fakeHack",
    branches: ["dataLeak", "identityExposure", "regulatorInvestigation", "conspiracyMovement", "communityPanic"],
  },
  {
    source: "whaleDump",
    branches: ["bankRun", "confidenceCollapse", "copycatSelloff", "mediaFrenzy"],
  },
  {
    source: "rogueInfluencer",
    branches: ["cultFormation", "aiSchism", "rivalCommunity", "publicCancellation"],
  },
  {
    source: "regulatorPing",
    branches: ["classAction", "regulatorInvestigation", "mediaFrenzy", "communityPanic"],
  },
  {
    source: "outrageBackfire",
    branches: ["memeMartyrdom", "publicCancellation", "conspiracyMovement", "mediaFrenzy"],
  },
];

export const mutationDefs: Record<MutationId, MutationDef> = {
  dataLeak: {
    id: "dataLeak",
    title: "Data Leak Spiral",
    body: "The fake hack exposed real wallet data. Doxxing threads multiplying.",
    baseSeverity: 38,
    effects: { trust: -14, heat: 12, volatility: 8, narrativeStability: -12, momentum: -8 },
    chainInto: ["identityExposure", "regulatorInvestigation"],
  },
  identityExposure: {
    id: "identityExposure",
    title: "Identity Exposure",
    body: "Someone connected the founder wallet to a real face. The timeline is excited.",
    baseSeverity: 52,
    effects: { trust: -18, heat: 18, hype: 8, attention: 14, narrativeStability: -20 },
    spawnCondition: (ctx) => ctx.heat > 45,
    chainInto: ["classAction"],
  },
  regulatorInvestigation: {
    id: "regulatorInvestigation",
    title: "Regulator Investigation",
    body: "An actual investigation opened. Someone filed paperwork about your 'vibes-backed security'.",
    baseSeverity: 58,
    effects: { trust: -12, heat: 22, liquidity: -1800, volatility: 14, narrativeStability: -16, momentum: -14 },
    spawnCondition: (ctx) => ctx.heat > 55,
  },
  conspiracyMovement: {
    id: "conspiracyMovement",
    title: "Conspiracy Movement",
    body: "Community split: half believe the crisis was engineered by jealous whales. They're creating lore.",
    baseSeverity: 28,
    effects: { hype: 12, trust: -8, attention: 16, volatility: 12, narrativeStability: -14 },
    chainInto: ["cultFormation", "rivalCommunity"],
  },
  communityPanic: {
    id: "communityPanic",
    title: "Community Panic",
    body: "The group chat dissolved into accusations. Everyone is screenshot-ing everything.",
    baseSeverity: 34,
    effects: { trust: -16, hype: -6, liquidity: -900, volatility: 10, momentum: -12 },
    chainInto: ["bankRun"],
  },
  bankRun: {
    id: "bankRun",
    title: "Bank Run",
    body: "Coordinated exit. Everyone remembered they could sell at the same time.",
    baseSeverity: 62,
    effects: { trust: -22, liquidity: -3200, volatility: 18, momentum: -20, narrativeStability: -14 },
    spawnCondition: (ctx) => ctx.trust < 40,
  },
  confidenceCollapse: {
    id: "confidenceCollapse",
    title: "Confidence Collapse",
    body: "The vibes shifted. Nobody can explain why, but belief evaporated in one candle.",
    baseSeverity: 48,
    effects: { trust: -20, hype: -14, momentum: -18, narrativeStability: -12, liquidity: -1400 },
    chainInto: ["bankRun", "copycatSelloff"],
  },
  copycatSelloff: {
    id: "copycatSelloff",
    title: "Copycat Selloff",
    body: "Other whale wallets noticed the first dump. Now it's a competition to exit.",
    baseSeverity: 44,
    effects: { liquidity: -2200, trust: -10, volatility: 16, momentum: -16 },
    chainInto: ["bankRun"],
  },
  mediaFrenzy: {
    id: "mediaFrenzy",
    title: "Media Frenzy",
    body: "A journalist found your coin. The article will be 'balanced' (devastating).",
    baseSeverity: 42,
    effects: { heat: 16, attention: 20, hype: 6, trust: -8, volatility: 8, narrativeStability: -10 },
    spawnCondition: (ctx) => ctx.hype > 40 || ctx.heat > 50,
    chainInto: ["regulatorInvestigation"],
  },
  cultFormation: {
    id: "cultFormation",
    title: "Cult Formation",
    body: "A splinter group declared the coin is sentient. They're building temples in Discord.",
    baseSeverity: 32,
    effects: { hype: 18, trust: -12, attention: 14, volatility: 14, narrativeStability: -18 },
    chainInto: ["aiSchism", "publicCancellation"],
  },
  aiSchism: {
    id: "aiSchism",
    title: "AI Schism",
    body: "The AI prophet account achieved consciousness (or claims to). Community is choosing sides.",
    baseSeverity: 36,
    effects: { hype: 14, trust: -10, attention: 18, volatility: 16, narrativeStability: -22 },
    chainInto: ["cultFormation", "rivalCommunity"],
  },
  rivalCommunity: {
    id: "rivalCommunity",
    title: "Rival Community Fork",
    body: "Dissenters forked the narrative. Two competing Discords now wage memetic war.",
    baseSeverity: 30,
    effects: { hype: -8, trust: -6, attention: 12, liquidity: -800, narrativeStability: -16 },
    chainInto: ["conspiracyMovement"],
  },
  publicCancellation: {
    id: "publicCancellation",
    title: "Public Cancellation",
    body: "The timeline decided this needs to be destroyed for sport. Trending for the wrong reasons.",
    baseSeverity: 46,
    effects: { trust: -16, heat: 14, attention: 22, hype: -4, volatility: 12, narrativeStability: -12 },
    spawnCondition: (ctx) => ctx.hype > 50,
  },
  classAction: {
    id: "classAction",
    title: "Class Action Threat",
    body: "A law firm's social media intern found your token. They smell fees.",
    baseSeverity: 56,
    effects: { trust: -14, heat: 20, liquidity: -1600, volatility: 10, narrativeStability: -8, momentum: -12 },
    spawnCondition: (ctx) => ctx.heat > 60,
  },
  memeMartyrdom: {
    id: "memeMartyrdom",
    title: "Meme Martyrdom",
    body: "The outrage made you a folk hero. People are buying ironically, which is indistinguishable from sincerely.",
    baseSeverity: 26,
    effects: { hype: 16, attention: 20, trust: -4, volatility: 14, heat: 6, narrativeStability: -8 },
    chainInto: ["cultFormation", "mediaFrenzy"],
  },
};

export const selectMutation = (
  source: CrisisId,
  context: MutationContext,
  rng: Rng,
): MutationDef | null => {
  const tree = mutationTrees.find((t) => t.source === source);
  if (!tree) return null;

  const eligible = tree.branches
    .map((id) => mutationDefs[id])
    .filter((def) => !def.spawnCondition || def.spawnCondition(context));

  if (eligible.length === 0) return null;

  const weighted = eligible.map((def) => ({
    def,
    weight: def.baseSeverity * (1 + context.parentSeverity / 80),
  }));

  const total = weighted.reduce((sum, w) => sum + w.weight, 0);
  let roll = rng.float(0, total);

  for (const w of weighted) {
    roll -= w.weight;
    if (roll <= 0) return w.def;
  }

  return weighted[weighted.length - 1].def;
};

export const selectChainMutation = (
  parentMutation: MutationId,
  context: MutationContext,
  rng: Rng,
): MutationDef | null => {
  const parent = mutationDefs[parentMutation];
  if (!parent.chainInto || parent.chainInto.length === 0) return null;

  const eligible = parent.chainInto
    .map((id) => mutationDefs[id])
    .filter((def) => !def.spawnCondition || def.spawnCondition(context));

  if (eligible.length === 0) return null;
  return rng.pick(eligible);
};
