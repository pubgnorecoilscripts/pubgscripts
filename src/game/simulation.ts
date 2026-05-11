import {
  actionDefinitions,
  influencerTemplates,
  makeCrisis,
  makeMutatedCrisis,
  narrativeSeeds,
  postFragments,
  tickerSeeds,
  traitBehaviors,
} from "./content";
import { createFactionStates, tickFactions } from "./factions";
import { createFrenzyState, frenzyMultipliers, recordFrenzyAction, tickFrenzy } from "./frenzy";
import { selectChainMutation, selectMutation, type MutationContext } from "./mutations";
import { clamp, createRng, normalizeSeed, type Rng } from "./rng";
import { createSeason, tickSeason } from "./seasons";
import type {
  ActionId,
  CoinState,
  Crisis,
  CrisisId,
  DelayedRisk,
  EcosystemScar,
  Influencer,
  MarketEvent,
  MarketPoint,
  MomentumChain,
  Narrative,
  Sentiment,
  SocialPost,
  WorldState,
} from "./types";

const SUPPLY = 1_000_000_000;
const MAX_FEED_ITEMS = 64;
const MAX_MARKET_POINTS = 120;
const MAX_ACTIVE_CRISES = 4;

const terminalLine = (tick: number, message: string) => `T+${tick.toString().padStart(3, "0")} // ${message}`;

const cloneWorld = (world: WorldState): WorldState => ({
  ...world,
  activeNarrative: world.activeNarrative ? cloneNarrative(world.activeNarrative) : null,
  deadNarratives: [...world.deadNarratives],
  influencers: world.influencers.map((i) => ({ ...i })),
  feed: [...world.feed],
  factions: world.factions.map((f) => ({ ...f })),
  market: [...world.market],
  scars: [...world.scars],
  season: { ...world.season, effects: { ...world.season.effects } },
  stats: { ...world.stats },
  terminal: [...world.terminal],
});

const cloneNarrative = (n: Narrative): Narrative => ({
  ...n,
  coin: { ...n.coin },
  cooldowns: { ...n.cooldowns },
  momentumChains: n.momentumChains.map((c) => ({ ...c, synergy: { ...c.synergy } })),
  crises: n.crises.map((c) => ({ ...c, choices: c.choices.map((ch) => ({ ...ch, effects: { ...ch.effects } })) })),
  delayedRisks: [...n.delayedRisks],
  frenzy: { ...n.frenzy, comboActions: [...n.frenzy.comboActions] },
});

const addFeed = (world: WorldState, posts: SocialPost[]) => {
  world.feed = [...posts, ...world.feed].slice(0, MAX_FEED_ITEMS);
};

const addTerminal = (world: WorldState, message: string) => {
  world.terminal = [terminalLine(world.tick, message), ...world.terminal].slice(0, 28);
};

const pushMarketPoint = (world: WorldState, event?: MarketEvent) => {
  const coin = world.activeNarrative?.coin;
  if (!coin) return;

  const point: MarketPoint = {
    tick: world.tick,
    price: coin.price,
    liquidity: coin.liquidity,
    volume: coin.volume,
    priceVelocity: coin.priceVelocity,
    momentum: coin.momentum,
    hype: coin.hype,
    trust: coin.trust,
    narrativeStability: coin.narrativeStability,
    event,
  };

  world.market = [...world.market, point].slice(-MAX_MARKET_POINTS);
};

const makePost = (
  rng: Rng,
  tick: number,
  sentiment: Sentiment,
  impact: number,
  author?: string,
  body?: string,
  effects?: SocialPost["effects"],
): SocialPost => ({
  id: `${tick}-${sentiment}-${rng.int(1000, 9999)}`,
  tick,
  author: author ?? rng.pick(["@anon", "@bagwatcher", "@terminallyLong", "@doomscroll_cfo", "@chain_janitor"]),
  body: body ?? rng.pick(postFragments[sentiment]),
  sentiment,
  impact,
  effects: effects ?? postEffects(sentiment, impact),
});

const postEffects = (sentiment: Sentiment, impact: number): SocialPost["effects"] => {
  const scaled = clamp(impact / 22, -4, 4);
  switch (sentiment) {
    case "bullish":
      return { hype: 0.8 + scaled * 0.5, attention: 0.7, momentum: 0.9 + scaled * 0.4, volatility: 0.4 };
    case "bearish":
      return { trust: -1.4 + Math.min(0, scaled * 0.6), volatility: 1.6, momentum: -1.4, narrativeStability: -1.0 };
    case "unhinged":
      return { hype: 1.2, attention: 1.6, heat: 0.8, volatility: 1.4, narrativeStability: -1.4, momentum: scaled * 0.5 };
    case "regulatory":
      return { trust: -1.8, heat: 2.0, volatility: 1.2, narrativeStability: -0.9, momentum: -1.0 };
    case "bot":
      return { hype: 0.6, attention: 0.9, trust: -0.4, heat: 0.5, narrativeStability: -0.3 };
  }
};

const applyFeedEffects = (coin: CoinState, posts: SocialPost[]) => {
  for (const post of posts) {
    applyCoinEffects(coin, post.effects);
  }
};

const deriveSentiment = (coin: CoinState, rng: Rng): Sentiment => {
  if (coin.heat > 72 && rng.chance(0.35)) return "regulatory";
  if (coin.trust < 28 && rng.chance(0.45)) return "bearish";
  if (coin.hype > 68 && rng.chance(0.45)) return "unhinged";
  return rng.chance(0.35) ? "bot" : "bullish";
};

const buildInfluencers = (rng: Rng): Influencer[] => {
  const pool = [...influencerTemplates];
  const selected: Influencer[] = [];
  while (selected.length < 4 && pool.length > 0) {
    const index = rng.int(0, pool.length - 1);
    const [template] = pool.splice(index, 1);
    selected.push({ ...template, id: `influencer-${selected.length}-${rng.int(100, 999)}` });
  }
  return selected;
};

const applyCoinEffects = (coin: CoinState, effects: Partial<CoinState>) => {
  coin.hype = clamp(coin.hype + (effects.hype ?? 0), 0, 100);
  coin.trust = clamp(coin.trust + (effects.trust ?? 0), 0, 100);
  coin.heat = clamp(coin.heat + (effects.heat ?? 0), 0, 100);
  coin.attention = clamp(coin.attention + (effects.attention ?? 0), 0, 100);
  coin.volatility = clamp(coin.volatility + (effects.volatility ?? 0), 5, 100);
  coin.narrativeStability = clamp(coin.narrativeStability + (effects.narrativeStability ?? 0), 0, 100);
  coin.momentum = clamp(coin.momentum + (effects.momentum ?? 0), -100, 100);
  coin.liquidity = Math.max(0, coin.liquidity + (effects.liquidity ?? 0));
  coin.marketCap = coin.price * SUPPLY;
};

export const createWorld = (seed = Date.now()): WorldState => {
  const rng = createRng(normalizeSeed(seed));
  return {
    seed: rng.state,
    rngState: rng.state,
    tick: 0,
    lastTickAt: Date.now(),
    cash: 5_000,
    influence: 0,
    reach: 0,
    notoriety: 0,
    activeNarrative: null,
    deadNarratives: [],
    influencers: buildInfluencers(rng),
    feed: [makePost(rng, 0, "bullish", 2, "@ecosystemDaemon", "Markets are open. Belief is tradable. Reality is a suggestion.")],
    factions: createFactionStates(rng),
    market: [],
    scars: [],
    season: createSeason(rng),
    stats: {
      totalInfluence: 0,
      peakInfluence: 0,
      narrativesLaunched: 0,
      narrativesCollapsed: 0,
      totalLiquidityExtracted: 0,
      ecosystemScars: 0,
      factionsCorrupted: 0,
      influencersBetraged: 0,
      crisesExploited: 0,
      frenziesTriggered: 0,
      dangerousPlays: 0,
      crisisFreezes: 0,
      seasonsSurvived: 0,
    },
    terminal: [terminalLine(0, "operator terminal online. the ecosystem awaits contamination.")],
    speed: "normal",
  };
};

export const launchNarrative = (world: WorldState, ticker: string, narrative: string): WorldState => {
  if (world.activeNarrative) return world;

  const next = cloneWorld(world);
  const rng = createRng(next.rngState);
  const price = rng.float(0.002, 0.009);

  const narr: Narrative = {
    id: `narr-${next.tick}-${rng.int(1000, 9999)}`,
    ticker: ticker.toUpperCase().replace(/[^A-Z0-9]/g, "").slice(0, 6) || "PUMP",
    narrative: narrative.trim() || "AI-powered engagement derivative",
    state: "active",
    launchedAt: next.tick,
    coin: {
      ticker: ticker.toUpperCase().replace(/[^A-Z0-9]/g, "").slice(0, 6) || "PUMP",
      narrative: narrative.trim() || "AI-powered engagement derivative",
      price,
      liquidity: rng.float(2_800, 4_200),
      marketCap: price * SUPPLY,
      volume: rng.float(320, 760),
      priceVelocity: 0,
      momentum: rng.float(6, 14),
      hype: rng.float(15, 22),
      trust: rng.float(68, 78),
      heat: rng.float(3, 8),
      attention: rng.float(15, 25),
      volatility: rng.float(12, 20),
      narrativeStability: rng.float(72, 84),
    },
    cooldowns: {},
    momentumChains: [],
    crises: [],
    delayedRisks: [],
    frenzy: createFrenzyState(),
    crisisFreezeCooldown: 0,
    influenceGenerated: 0,
  };

  next.activeNarrative = narr;
  next.stats.narrativesLaunched += 1;
  next.rngState = rng.state;
  addTerminal(next, `$${narr.ticker} launched: "${narr.narrative}". the ecosystem takes notice.`);
  addFeed(next, [
    makePost(rng, next.tick, "bullish", 4, "@launchTerminal", `$${narr.ticker} just appeared. narrative: "${narr.narrative}". too cursed to ignore.`),
  ]);
  pushMarketPoint(next);

  return next;
};

const activeChainPower = (chains: MomentumChain[]) =>
  chains.reduce(
    (t, c) => ({
      hypeVelocity: t.hypeVelocity + c.hypeVelocity * c.intensity,
      liquidityMultiplier: t.liquidityMultiplier + (c.liquidityMultiplier - 1) * c.intensity,
      heatAcceleration: t.heatAcceleration + c.heatAcceleration * c.intensity,
      volatilityAcceleration: t.volatilityAcceleration + c.volatilityAcceleration * c.intensity,
    }),
    { hypeVelocity: 0, liquidityMultiplier: 1, heatAcceleration: 0, volatilityAcceleration: 0 },
  );

const synergyMultiplierFor = (chains: MomentumChain[], actionId: ActionId) =>
  chains.reduce((m, c) => m * (c.synergy[actionId] ?? 1), 1);

const addMomentumChain = (narr: Narrative, actionId: ActionId, tick: number, multiplier: number) => {
  const action = actionDefinitions.find((a) => a.id === actionId);
  if (!action) return;
  const chain: MomentumChain = {
    id: `${actionId}-${tick}`,
    source: actionId,
    label: action.chain.label,
    startedAt: tick,
    expiresAt: tick + action.chain.duration,
    intensity: clamp(multiplier, 0.75, 2.2),
    hypeVelocity: action.chain.hypeVelocity,
    liquidityMultiplier: action.chain.liquidityMultiplier,
    heatAcceleration: action.chain.heatAcceleration,
    volatilityAcceleration: action.chain.volatilityAcceleration,
    synergy: action.chain.synergy,
  };
  narr.momentumChains = [chain, ...narr.momentumChains.filter((c) => c.source !== actionId)].slice(0, 6);
};

const scheduleDelayedRisk = (narr: Narrative, actionId: ActionId, tick: number, rng: Rng) => {
  const action = actionDefinitions.find((a) => a.id === actionId);
  if (!action?.delayedRisk) return;
  if (rng.chance(action.delayedRisk.chance)) {
    narr.delayedRisks.push({
      id: `risk-${actionId}-${tick}-${rng.int(100, 999)}`,
      sourceAction: actionId,
      triggerAt: tick + rng.int(action.delayedRisk.minDelay, action.delayedRisk.maxDelay),
      crisisType: action.delayedRisk.crisisType,
      description: action.delayedRisk.description,
    });
  }
};

export const performAction = (world: WorldState, actionId: ActionId): WorldState => {
  const narr = world.activeNarrative;
  if (!narr || narr.state !== "active") return world;

  const action = actionDefinitions.find((a) => a.id === actionId);
  if (!action || world.cash < action.cost || (narr.cooldowns[actionId] ?? 0) > 0) return world;

  const next = cloneWorld(world);
  const n = next.activeNarrative!;
  const coin = n.coin;
  const rng = createRng(next.rngState);

  const fMult = frenzyMultipliers(n.frenzy);
  const chainMult = synergyMultiplierFor(n.momentumChains, actionId);
  const effects = {
    ...action.effects,
    hype: action.effects.hype * chainMult * fMult.hypeGain,
    attention: action.effects.attention * chainMult,
    liquidity: action.effects.liquidity * chainMult * fMult.liquidityAccel,
    heat: action.effects.heat * (0.85 + chainMult * 0.28) * fMult.heatAccel,
    volatility: action.effects.volatility * (0.9 + chainMult * 0.22) * fMult.volatilityAmp,
    narrativeStability: action.effects.narrativeStability * (0.85 + chainMult * 0.2) * fMult.stabilityDrain,
  };

  next.cash -= action.cost;
  next.stats.dangerousPlays += action.dangerous ? 1 : 0;
  n.cooldowns[actionId] = action.cooldown;
  applyCoinEffects(coin, effects);
  addMomentumChain(n, actionId, next.tick, chainMult);

  if (action.dangerous) scheduleDelayedRisk(n, actionId, next.tick, rng);

  n.frenzy = recordFrenzyAction(n.frenzy, actionId, next.tick);
  if (n.frenzy.active && n.frenzy.totalFrenzies > (world.activeNarrative?.frenzy.totalFrenzies ?? 0)) {
    next.stats.frenziesTriggered += 1;
    addTerminal(next, "NARRATIVE FRENZY activated. everything accelerates. including collapse.");
    addFeed(next, [makePost(rng, next.tick, "unhinged", 20, "@momentumDaemon", "FRENZY MODE. The timeline is on fire and the fire is profitable.")]);
  }

  const influenceGain = (effects.hype + effects.attention) * 0.3 + (action.dangerous ? 5 : 0);
  next.influence += influenceGain;
  next.reach += effects.attention * 0.2;
  n.influenceGenerated += influenceGain;
  next.stats.totalInfluence += influenceGain;
  next.stats.peakInfluence = Math.max(next.stats.peakInfluence, next.influence);

  const posts = [
    makePost(rng, next.tick, actionId === "manufactureOutrage" || actionId === "syntheticOutrage" ? "unhinged" : "bullish", effects.hype, "@operator",
      `${action.command}${chainMult > 1.05 ? ` // combo x${chainMult.toFixed(2)}` : ""}${n.frenzy.active ? " // FRENZY" : ""}`),
  ];
  if (rng.chance(0.32 + coin.heat / 220)) posts.push(makePost(rng, next.tick, "bearish", -4));
  applyFeedEffects(coin, posts);
  addFeed(next, posts);
  addTerminal(next, `${action.label.toLowerCase()} executed.${action.dangerous ? " delayed consequences scheduled." : ""} influence +${Math.round(influenceGain)}.`);
  pushMarketPoint(next);
  next.rngState = rng.state;

  return next;
};

export const crisisFreeze = (world: WorldState): WorldState => {
  const narr = world.activeNarrative;
  if (!narr || narr.state !== "active" || narr.crisisFreezeCooldown > 0 || world.cash < 800 || narr.crises.length === 0) return world;

  const next = cloneWorld(world);
  const n = next.activeNarrative!;
  const rng = createRng(next.rngState);

  next.cash -= 800;
  n.crisisFreezeCooldown = 12;
  next.stats.crisisFreezes += 1;
  n.coin.heat = clamp(n.coin.heat + 6, 0, 100);
  for (const crisis of n.crises) {
    crisis.expiresAt += 4;
    crisis.frozen = true;
  }
  addTerminal(next, "crisis freeze deployed. mutation slowed. regulators noticed.");
  next.rngState = rng.state;
  return next;
};

export const resolveCrisis = (world: WorldState, crisisId: string, choiceId: string): WorldState => {
  const narr = world.activeNarrative;
  if (!narr || narr.state !== "active") return world;

  const crisis = narr.crises.find((c) => c.id === crisisId);
  const choice = crisis?.choices.find((c) => c.id === choiceId);
  if (!crisis || !choice) return world;

  const next = cloneWorld(world);
  const n = next.activeNarrative!;
  const rng = createRng(next.rngState);

  applyCoinEffects(n.coin, choice.effects);
  next.cash = Math.max(0, next.cash + (choice.effects.cash ?? 0));
  const influenceGain = (choice.effects.influence ?? 0) + 3;
  next.influence += influenceGain;
  next.stats.crisesExploited += 1;
  n.crises = n.crises.filter((c) => c.id !== crisisId);
  addTerminal(next, `${crisis.title.toLowerCase()} contained via "${choice.label.toLowerCase()}". influence +${Math.round(influenceGain)}.`);
  addFeed(next, [makePost(rng, next.tick, choice.effects.trust && choice.effects.trust > 0 ? "bullish" : "unhinged", crisis.severity, "@crisisPR", choice.description)]);
  pushMarketPoint(next);
  next.rngState = rng.state;
  return next;
};

export const rugPull = (world: WorldState): WorldState => {
  const narr = world.activeNarrative;
  if (!narr || narr.state !== "active") return world;

  const next = cloneWorld(world);
  const n = next.activeNarrative!;
  const coin = n.coin;
  const rng = createRng(next.rngState);

  const extractionRate = clamp(0.2 + coin.hype / 250 + coin.trust / 420 - coin.heat / 310, 0.08, 0.72);
  const extracted = coin.liquidity * extractionRate;
  next.cash += extracted;
  next.stats.totalLiquidityExtracted += extracted;
  next.notoriety += 15;

  n.state = "dead";
  n.collapsedAt = next.tick;
  coin.liquidity = Math.max(0, coin.liquidity - extracted);
  coin.trust = 0;
  coin.heat = 100;
  coin.momentum = -100;
  coin.narrativeStability = 0;

  const scar: EcosystemScar = {
    id: `scar-rug-${next.tick}`,
    tick: next.tick,
    type: "rugPull",
    title: `$${coin.ticker} Rug Pull`,
    description: `Operator extracted ${Math.round(extractionRate * 100)}% liquidity. Community decimated.`,
    effects: { trust: -4, heat: 3, volatility: 5 },
    factionEffects: { retailSwarm: -8, doomers: 6, regulators: 4 },
    permanent: true,
  };
  next.scars.push(scar);
  next.stats.ecosystemScars += 1;

  next.deadNarratives.push({ ticker: coin.ticker, influence: n.influenceGenerated, cause: "rug pull" });
  next.activeNarrative = null;
  next.influence += n.influenceGenerated * 0.2;

  addTerminal(next, `$${coin.ticker} rugged. ${Math.round(extractionRate * 100)}% extracted. ecosystem scarred permanently.`);
  addFeed(next, [
    makePost(rng, next.tick, "bearish", -90, "@exitLiquidity", `$${coin.ticker} founder wallet just discovered teleportation.`),
    makePost(rng, next.tick, "unhinged", 40, "@bagholderUnion", "THIS IS FINE. THE ROADMAP INCLUDED BETRAYAL."),
  ]);
  pushMarketPoint(next, { tick: next.tick, type: "scar", intensity: 1.0 });
  next.rngState = rng.state;
  return next;
};

export const pivotNarrative = (world: WorldState): WorldState => {
  const narr = world.activeNarrative;
  if (!narr || narr.state !== "active" || world.cash < 1_500) return world;

  const next = cloneWorld(world);
  const n = next.activeNarrative!;
  const rng = createRng(next.rngState);

  const old = n.coin.narrative;
  n.coin.narrative = rng.pick(narrativeSeeds);
  n.narrative = n.coin.narrative;
  next.cash -= 1_500;
  applyCoinEffects(n.coin, { hype: 15, attention: 16, trust: -12, heat: 8, volatility: 12, narrativeStability: -18, momentum: 12 });
  addTerminal(next, `pivoted from "${old}" to "${n.narrative}". nobody remembers yesterday.`);
  addFeed(next, [makePost(rng, next.tick, "unhinged", 16, "@narrativeDesk", "The premise changed twice. We are so early.")]);
  pushMarketPoint(next);
  next.rngState = rng.state;
  return next;
};

const collapseNarrative = (world: WorldState, rng: Rng, cause: string): void => {
  const narr = world.activeNarrative;
  if (!narr) return;

  narr.state = "dead";
  narr.collapsedAt = world.tick;

  const scar: EcosystemScar = {
    id: `scar-collapse-${world.tick}`,
    tick: world.tick,
    type: "collapse",
    title: `$${narr.ticker} Collapsed`,
    description: cause,
    effects: { trust: -2, volatility: 3, heat: 2 },
    factionEffects: { doomers: 4, conspiracyAccounts: 3, retailSwarm: -3 },
    permanent: false,
  };
  world.scars.push(scar);
  world.stats.narrativesCollapsed += 1;
  world.stats.ecosystemScars += 1;
  world.deadNarratives.push({ ticker: narr.ticker, influence: narr.influenceGenerated, cause });
  world.influence += narr.influenceGenerated * 0.1;
  world.notoriety += 5;

  addTerminal(world, `$${narr.ticker} collapsed: ${cause}. your influence persists. launch another.`);
  addFeed(world, [
    makePost(rng, world.tick, "bearish", -60, "@postmortem", `$${narr.ticker} is now a cautionary screenshot. Operator influence remains.`),
  ]);
  pushMarketPoint(world, { tick: world.tick, type: "crash", intensity: 0.8 });

  world.activeNarrative = null;
};

export const advanceTick = (world: WorldState): WorldState => {
  const next = cloneWorld(world);
  const rng = createRng(next.rngState);
  next.tick += 1;
  next.lastTickAt = Date.now();

  const { season: newSeason, changed: seasonChanged } = tickSeason(next.season, rng);
  next.season = newSeason;
  if (seasonChanged) {
    next.stats.seasonsSurvived += 1;
    addTerminal(next, `season shift: ${next.season.label}. the macro environment mutates.`);
    addFeed(next, [makePost(rng, next.tick, "regulatory", 8, "@macroShift", `${next.season.description}`)]);
  }

  const { factions: updatedFactions, effects: factionEffects, posts: factionPosts } = tickFactions(next.factions, next.activeNarrative?.coin ?? dummyCoin(), rng);
  next.factions = updatedFactions;

  if (factionPosts.length > 0) {
    const socialPosts = factionPosts.map((fp) => ({ ...fp, id: `faction-${next.tick}-${rng.int(1000, 9999)}`, tick: next.tick, factionSource: fp.author }));
    addFeed(next, socialPosts);
  }

  const narr = next.activeNarrative;
  if (!narr || narr.state !== "active") {
    next.influence = Math.max(0, next.influence * 0.998 - 0.1);
    next.rngState = rng.state;
    return next;
  }

  const coin = narr.coin;
  narr.momentumChains = narr.momentumChains.filter((c) => c.expiresAt > next.tick);
  narr.frenzy = tickFrenzy(narr.frenzy);
  narr.crisisFreezeCooldown = Math.max(0, narr.crisisFreezeCooldown - 1);
  for (const crisis of narr.crises) { if (crisis.frozen) crisis.frozen = false; }

  for (const key of Object.keys(narr.cooldowns) as ActionId[]) {
    narr.cooldowns[key] = Math.max(0, (narr.cooldowns[key] ?? 0) - 1);
  }

  applyCoinEffects(coin, factionEffects);
  escalateActiveCrises(next, narr, rng);
  resolveExpiredCrises(next, narr, rng);
  processDelayedRisks(next, narr, rng);
  tickInfluencers(next, rng);

  const fMult = frenzyMultipliers(narr.frenzy);
  const crisisDrag = narr.crises.reduce((s, c) => s + c.severity, 0);
  const chainPower = activeChainPower(narr.momentumChains);
  const narrativePenalty = (100 - coin.narrativeStability) * 0.22;
  const beliefPressure = coin.hype * 0.5 + coin.attention * 0.3 + coin.trust * 0.18 + coin.momentum * 0.34 + chainPower.hypeVelocity * 1.8 - coin.heat * 0.42 - crisisDrag * 0.24 - narrativePenalty;
  const seasonVol = next.season.effects.volatilityMod;
  const liquidityFlow = beliefPressure * rng.float(12, 28) * chainPower.liquidityMultiplier * fMult.liquidityAccel + rng.float(-650, 900);
  const whaleTrade = rng.chance(0.08 + coin.volatility / 700) ? rng.pick([-1, 1]) * rng.float(0.08, 0.34 + coin.volatility / 260) : 0;
  const panicSelloff = coin.trust < 30 && rng.chance(0.12 + (30 - coin.trust) / 120) ? -rng.float(0.18, 0.55) : 0;
  const euphoriaPump = coin.momentum > 55 && rng.chance(0.1 + coin.attention / 650) ? rng.float(0.16, 0.48) : 0;
  const volatilityShock = rng.float(-coin.volatility, coin.volatility) / 92 * seasonVol;
  const frenzyBoost = narr.frenzy.active ? rng.float(0.02, 0.08) * narr.frenzy.intensity : 0;
  const priceMove = clamp((beliefPressure - 34) / 105 + volatilityShock - coin.heat / 430 + whaleTrade + panicSelloff + euphoriaPump + frenzyBoost, -0.68, 1.1);
  const absoluteMove = Math.abs(priceMove);

  let marketEvent: MarketEvent | undefined;
  if (whaleTrade !== 0) marketEvent = { tick: next.tick, type: "whale", intensity: Math.abs(whaleTrade) };
  else if (panicSelloff < -0.3) marketEvent = { tick: next.tick, type: "panic", intensity: Math.abs(panicSelloff) };
  else if (euphoriaPump > 0.3) marketEvent = { tick: next.tick, type: "euphoria", intensity: euphoriaPump };
  else if (narr.frenzy.active && narr.frenzy.intensity > 1.5) marketEvent = { tick: next.tick, type: "frenzy", intensity: narr.frenzy.intensity };

  coin.liquidity = Math.max(0, coin.liquidity + liquidityFlow);
  coin.price = Math.max(0.00001, coin.price * (1 + priceMove));
  coin.marketCap = coin.price * SUPPLY;
  coin.volume = Math.max(80, coin.liquidity * (0.05 + absoluteMove * rng.float(0.75, 1.6) + coin.attention / 900) * fMult.feedVelocity);
  coin.priceVelocity = priceMove;
  coin.hype = clamp(coin.hype * 0.9 + coin.attention * 0.075 + chainPower.hypeVelocity * fMult.hypeGain + rng.float(-3.5, 4.5), 0, 100);
  coin.attention = clamp(coin.attention * 0.875 + coin.hype * 0.055 + absoluteMove * 10 + rng.float(-2, 5.5), 0, 100);

  const earlyShield = Math.max(0.3, Math.min(1, (next.tick - narr.launchedAt - 3) / 10));
  const trustRecovery = coin.heat < 25 && coin.volatility < 40 ? 0.6 : 0;
  const trustFloor = next.season.effects.trustFloor;
  coin.trust = clamp(
    coin.trust - coin.heat * 0.028 * earlyShield - Math.max(0, coin.volatility - 55) * 0.03 * earlyShield - Math.max(0, absoluteMove - 0.35) * 5 * earlyShield + trustRecovery + rng.float(-1.8, 2.4),
    trustFloor, 100,
  );
  coin.heat = clamp(coin.heat * 0.95 + Math.max(0, coin.hype - 58) * 0.04 + chainPower.heatAcceleration * fMult.heatAccel + rng.float(-1.3, 2.4), 0, 100);
  coin.volatility = clamp(coin.volatility * 0.9 + absoluteMove * 54 * seasonVol + chainPower.volatilityAcceleration * fMult.volatilityAmp + rng.float(-2.4, 3.2), 5, 100);
  coin.momentum = clamp(coin.momentum * 0.84 + priceMove * 44 + chainPower.hypeVelocity * 1.7 + rng.float(-5, 5), -100, 100);
  coin.narrativeStability = clamp(
    coin.narrativeStability - coin.heat * 0.026 - narr.crises.length * 1.2 - Math.max(0, coin.hype - coin.trust) * 0.018 - (narr.frenzy.active ? narr.frenzy.intensity * 1.4 * fMult.stabilityDrain : 0) + rng.float(-1.3, 1.8),
    0, 100,
  );
  next.cash += Math.max(0, coin.liquidity * 0.006 * (coin.trust / 100));

  const influenceTick = (coin.hype * 0.01 + coin.attention * 0.008 + Math.max(0, coin.momentum) * 0.005) * (narr.frenzy.active ? 2 : 1);
  next.influence += influenceTick;
  next.reach += coin.attention * 0.005;
  narr.influenceGenerated += influenceTick;
  next.stats.totalInfluence += influenceTick;
  next.stats.peakInfluence = Math.max(next.stats.peakInfluence, next.influence);

  if (whaleTrade !== 0) {
    const whalePosts = [makePost(rng, next.tick, whaleTrade > 0 ? "bullish" : "bearish", whaleTrade * 40, "@whaleScanner", whaleTrade > 0 ? "Whale wallet aped so hard the chart flinched." : "Whale wallet dumped into retail optimism.")];
    applyFeedEffects(coin, whalePosts);
    addFeed(next, whalePosts);
  }

  const sentiment = deriveSentiment(coin, rng);
  const postCount = narr.frenzy.active ? Math.min(4, 2 + Math.floor(narr.frenzy.intensity)) : 2;
  const posts: SocialPost[] = [];
  for (let i = 0; i < postCount; i++) {
    posts.push(makePost(rng, next.tick, i === 0 ? sentiment : rng.chance(0.3) ? "unhinged" : sentiment, beliefPressure * (1 - i * 0.25)));
  }
  applyFeedEffects(coin, posts);
  addFeed(next, posts);

  const crisis = spawnCrisis(next, narr, rng, fMult);
  if (crisis) {
    narr.crises.push(crisis);
    addTerminal(next, `crisis spawned: ${crisis.title}. respond before it mutates.`);
    addFeed(next, [makePost(rng, next.tick, "regulatory", -crisis.severity, "@riskDaemon", crisis.body)]);
  }

  if (coin.trust <= 1 || coin.liquidity <= 150 || coin.narrativeStability <= 1 || (coin.heat >= 98 && rng.chance(0.34))) {
    const cause = coin.trust <= 1 ? "trust collapse" : coin.liquidity <= 150 ? "liquidity death" : coin.narrativeStability <= 1 ? "narrative disintegration" : "regulatory extinction";
    collapseNarrative(next, rng, cause);
  }

  pushMarketPoint(next, marketEvent);
  next.rngState = rng.state;
  return next;
};

export const simulateOffline = (world: WorldState, maxTicks = 50): WorldState => {
  let state = world;
  const elapsed = Math.floor((Date.now() - state.lastTickAt) / 1400);
  const ticksToSim = Math.min(elapsed, maxTicks);
  for (let i = 0; i < ticksToSim; i++) {
    state = advanceTick(state);
  }
  return state;
};

export const randomLaunchConfig = (seed: number) => {
  const rng = createRng(seed);
  return { ticker: rng.pick(tickerSeeds), narrative: rng.pick(narrativeSeeds) };
};

function dummyCoin(): CoinState {
  return { ticker: "", narrative: "", price: 0, liquidity: 0, marketCap: 0, volume: 0, priceVelocity: 0, momentum: 0, hype: 20, trust: 50, heat: 10, attention: 20, volatility: 15, narrativeStability: 60 };
}

function escalateActiveCrises(world: WorldState, narr: Narrative, rng: Rng) {
  const coin = narr.coin;
  if (narr.crises.length === 0) return;
  const chainPressure = narr.momentumChains.reduce((s, c) => s + c.intensity, 0);
  narr.crises = narr.crises.map((crisis) => {
    const gain = 0.8 + coin.heat / 110 + Math.max(0, 45 - coin.narrativeStability) / 65 + chainPressure * 0.22;
    return { ...crisis, severity: clamp(crisis.severity + gain, 8, 95) };
  });
}

function resolveExpiredCrises(world: WorldState, narr: Narrative, rng: Rng) {
  const coin = narr.coin;
  const expired = narr.crises.filter((c) => world.tick >= c.expiresAt);
  if (expired.length === 0) return;

  for (const crisis of expired) {
    applyCoinEffects(coin, { hype: -crisis.severity * 0.18, trust: -crisis.severity * 0.28, heat: crisis.severity * 0.2, liquidity: -crisis.severity * rng.float(28, 52), volatility: crisis.severity * 0.16, narrativeStability: -crisis.severity * 0.24, momentum: -crisis.severity * 0.12 });
    addFeed(world, [makePost(rng, world.tick, "bearish", -crisis.severity, "@panicDesk", `${crisis.title} metastasized.`)]);
    addTerminal(world, `${crisis.title.toLowerCase()} mutating into something worse.`);

    const ctx: MutationContext = { parentSeverity: crisis.severity, heat: coin.heat, trust: coin.trust, hype: coin.hype, narrativeStability: coin.narrativeStability, activeCrisisCount: narr.crises.length, tick: world.tick };
    const gen = (crisis.mutationGeneration ?? 0) + 1;
    const mutation = crisis.mutationSource ? selectChainMutation(crisis.mutationSource, ctx, rng) : selectMutation(crisis.type, ctx, rng);
    if (mutation && narr.crises.length < MAX_ACTIVE_CRISES + 1) {
      const mc = makeMutatedCrisis(mutation.title, mutation.body, `mut-${world.tick}-${rng.int(100, 999)}`, world.tick, mutation.baseSeverity + crisis.severity * 0.3, crisis.type, mutation.id, gen);
      mc.narrativeId = narr.id;
      narr.crises.push(mc);
      world.stats.ecosystemScars += 0;
      applyCoinEffects(coin, mutation.effects);
      addTerminal(world, `mutation: "${crisis.title.toLowerCase()}" → "${mutation.title.toLowerCase()}".`);
    }
  }
  narr.crises = narr.crises.filter((c) => world.tick < c.expiresAt);
}

function processDelayedRisks(world: WorldState, narr: Narrative, rng: Rng) {
  const triggered = narr.delayedRisks.filter((r) => world.tick >= r.triggerAt);
  if (triggered.length === 0) return;
  for (const risk of triggered) {
    if (risk.crisisType && narr.crises.length < MAX_ACTIVE_CRISES) {
      const crisis = makeCrisis(risk.crisisType, `del-${world.tick}-${rng.int(100, 999)}`, world.tick, rng.float(35, 60));
      crisis.narrativeId = narr.id;
      narr.crises.push(crisis);
      addTerminal(world, `delayed consequence: ${risk.description.toLowerCase()}`);
      addFeed(world, [makePost(rng, world.tick, "bearish", -30, "@consequenceDaemon", risk.description)]);
    } else {
      applyCoinEffects(narr.coin, { trust: -18, heat: 12, volatility: 10, narrativeStability: -14, momentum: -8 });
      addTerminal(world, `risk materialized: ${risk.description.toLowerCase()}`);
    }
  }
  narr.delayedRisks = narr.delayedRisks.filter((r) => world.tick < r.triggerAt);
}

function tickInfluencers(world: WorldState, rng: Rng) {
  const coin = world.activeNarrative?.coin;
  if (!coin) return;
  for (const inf of world.influencers) {
    if (inf.status === "cancelled" || inf.status === "martyred") continue;
    const behavior = traitBehaviors[inf.trait];
    inf.autonomy = clamp(inf.autonomy + behavior.autonomyGrowth * 0.4 + rng.float(-0.5, 1.2), 0, 100);
    inf.audience = Math.max(0, inf.audience + inf.audience * behavior.audienceGrowthRate * 0.008 * rng.float(0.5, 1.5));
    inf.alignment = clamp(inf.alignment - coin.heat * 0.012 - inf.autonomy * 0.015 + rng.float(-2.5, 2), 0, 100);

    if (inf.status === "loyal" && inf.alignment < behavior.goRogueThreshold && rng.chance(inf.volatility / 200)) {
      inf.status = "rogue";
      world.stats.influencersBetraged += 1;
      applyCoinEffects(coin, { hype: 7, heat: 8, trust: -9, volatility: 7, narrativeStability: -8, momentum: -4 });
      addFeed(world, [makePost(rng, world.tick, "unhinged", 8, inf.handle, "I was never paid enough to pretend this was sane.")]);
      addTerminal(world, `${inf.handle} went rogue. their audience follows them, not you.`);
    }

    if (inf.status === "loyal" && inf.autonomy > 60 && rng.chance(behavior.disobedienceChance * 0.5)) {
      applyCoinEffects(coin, { narrativeStability: -4, attention: 3 });
      addFeed(world, [makePost(rng, world.tick, "unhinged", 4, inf.handle, "Actually... I have my own take on this narrative.")]);
    }
  }
}

function spawnCrisis(world: WorldState, narr: Narrative, rng: Rng, fMult: ReturnType<typeof frenzyMultipliers>): Crisis | undefined {
  const coin = narr.coin;
  if (narr.crises.length >= MAX_ACTIVE_CRISES) return undefined;
  const chainPressure = narr.momentumChains.reduce((s, c) => s + c.intensity, 0);
  const instability = (100 - coin.narrativeStability) / 100;
  const ticksSinceLaunch = world.tick - narr.launchedAt;
  const earlyDamper = Math.min(1, ticksSinceLaunch / 12);
  const seasonMod = world.season.effects.crisisRateMod;
  const chance = (0.04 + coin.heat / 280 + coin.volatility / 520 + Math.max(0, coin.hype - coin.trust) / 380 + chainPressure * 0.03 + instability * 0.08) * fMult.crisisSpawnRate * earlyDamper * seasonMod;
  if (!rng.chance(chance)) return undefined;

  const pool: CrisisId[] = coin.heat > 60 || coin.narrativeStability < 35 ? ["regulatorPing", "outrageBackfire", "fakeHack", "rogueInfluencer"] : ["rogueInfluencer", "whaleDump", "fakeHack"];
  const type = rng.pick(pool);
  const crisis = makeCrisis(type, `crisis-${world.tick}-${rng.int(100, 999)}`, world.tick, rng.float(18, 42) + chainPressure * 4 + instability * 16);
  crisis.narrativeId = narr.id;
  return crisis;
}
