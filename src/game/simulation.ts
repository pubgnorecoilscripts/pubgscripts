import {
  actionDefinitions,
  influencerTemplates,
  makeCrisis,
  narrativeSeeds,
  postFragments,
  tickerSeeds,
} from "./content";
import { clamp, createRng, normalizeSeed, type Rng } from "./rng";
import type {
  ActionId,
  CoinConfig,
  CoinState,
  Crisis,
  CrisisId,
  GameState,
  Influencer,
  MarketPoint,
  MomentumChain,
  Sentiment,
  SocialPost,
  TickResult,
} from "./types";

const SUPPLY = 1_000_000_000;
const MAX_FEED_ITEMS = 64;
const MAX_MARKET_POINTS = 120;
const MAX_ACTIVE_CRISES = 3;

const terminalLine = (tick: number, message: string) => `T+${tick.toString().padStart(3, "0")} // ${message}`;

const cloneState = (state: GameState): GameState => ({
  ...state,
  coin: state.coin ? { ...state.coin } : null,
  influencers: state.influencers.map((influencer) => ({ ...influencer })),
  feed: state.feed.map((post) => ({ ...post })),
  crises: state.crises.map((crisis) => ({
    ...crisis,
    choices: crisis.choices.map((choice) => ({ ...choice, effects: { ...choice.effects } })),
  })),
  momentumChains: state.momentumChains.map((chain) => ({ ...chain, synergy: { ...chain.synergy } })),
  market: state.market.map((point) => ({ ...point })),
  cooldowns: { ...state.cooldowns },
  stats: { ...state.stats },
  terminal: [...state.terminal],
});

const addFeed = (state: GameState, posts: SocialPost[]) => {
  state.feed = [...posts, ...state.feed].slice(0, MAX_FEED_ITEMS);
};

const addTerminal = (state: GameState, message: string) => {
  state.terminal = [terminalLine(state.tick, message), ...state.terminal].slice(0, 28);
};

const pushMarketPoint = (state: GameState) => {
  if (!state.coin) {
    return;
  }

  const point: MarketPoint = {
    tick: state.tick,
    price: state.coin.price,
    liquidity: state.coin.liquidity,
    volume: state.coin.volume,
    priceVelocity: state.coin.priceVelocity,
    momentum: state.coin.momentum,
    hype: state.coin.hype,
    trust: state.coin.trust,
    narrativeStability: state.coin.narrativeStability,
  };

  state.market = [...state.market, point].slice(-MAX_MARKET_POINTS);
  state.stats.peakMarketCap = Math.max(state.stats.peakMarketCap, state.coin.marketCap);
  state.stats.peakHype = Math.max(state.stats.peakHype, state.coin.hype);
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
  const scaled = clamp(impact / 18, -5, 5);

  switch (sentiment) {
    case "bullish":
      return { hype: 1.4 + scaled, attention: 1.2, momentum: 1.6 + scaled * 0.7, volatility: 0.8 };
    case "bearish":
      return { trust: -2.4 + Math.min(0, scaled), volatility: 2.8, momentum: -2.5, narrativeStability: -1.8 };
    case "unhinged":
      return { hype: 2.2, attention: 2.8, heat: 1.5, volatility: 2.4, narrativeStability: -2.6, momentum: scaled };
    case "regulatory":
      return { trust: -3, heat: 3.4, volatility: 2.2, narrativeStability: -1.6, momentum: -1.8 };
    case "bot":
      return { hype: 1.1, attention: 1.6, trust: -0.8, heat: 0.9, narrativeStability: -0.6 };
  }
};

const applyFeedEffects = (state: GameState, posts: SocialPost[]) => {
  if (!state.coin) {
    return;
  }

  for (const post of posts) {
    applyCoinEffects(state.coin, post.effects);
  }
};

const deriveSentiment = (coin: CoinState, rng: Rng): Sentiment => {
  if (coin.heat > 72 && rng.chance(0.35)) {
    return "regulatory";
  }

  if (coin.trust < 28 && rng.chance(0.45)) {
    return "bearish";
  }

  if (coin.hype > 68 && rng.chance(0.45)) {
    return "unhinged";
  }

  return rng.chance(0.35) ? "bot" : "bullish";
};

const buildInfluencers = (rng: Rng): Influencer[] => {
  const pool = [...influencerTemplates];
  const selected: Influencer[] = [];

  while (selected.length < 3 && pool.length > 0) {
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

export const randomCoinConfig = (seed = Date.now()): CoinConfig => {
  const rng = createRng(seed);
  return {
    ticker: rng.pick(tickerSeeds),
    narrative: rng.pick(narrativeSeeds),
    seed: normalizeSeed(seed),
  };
};

export const createSetupState = (seed = Date.now()): GameState => {
  const rng = createRng(seed);

  return {
    phase: "setup",
    seed: rng.state,
    rngState: rng.state,
    tick: 0,
    cash: 5_000,
    coin: null,
    influencers: buildInfluencers(rng),
    feed: [],
    crises: [],
    momentumChains: [],
    market: [],
    cooldowns: {},
    stats: {
      peakMarketCap: 0,
      peakHype: 0,
      crisesSurvived: 0,
      actionsTaken: 0,
      rugPulls: 0,
    },
    terminal: [terminalLine(0, "boot sequence armed. manufacture belief before reality reconciles.")],
  };
};

export const startRun = (config: CoinConfig): GameState => {
  const rng = createRng(config.seed);
  const price = rng.float(0.002, 0.009);
  const state = createSetupState(config.seed);

  state.phase = "running";
  state.coin = {
    ticker: config.ticker.toUpperCase().replace(/[^A-Z0-9]/g, "").slice(0, 6) || "PUMP",
    narrative: config.narrative.trim() || "AI-powered engagement derivative",
    price,
    liquidity: rng.float(2_100, 3_400),
    marketCap: price * SUPPLY,
    volume: rng.float(320, 760),
    priceVelocity: 0,
    momentum: rng.float(4, 12),
    hype: rng.float(18, 28),
    trust: rng.float(48, 62),
    heat: rng.float(5, 12),
    attention: rng.float(20, 34),
    volatility: rng.float(18, 28),
    narrativeStability: rng.float(58, 72),
  };
  state.rngState = rng.state;
  addTerminal(state, `$${state.coin.ticker} minted: ${state.coin.narrative}.`);
  addFeed(state, [
    makePost(
      rng,
      state.tick,
      "bullish",
      3,
      "@launchTerminal",
      `$${state.coin.ticker} just appeared with a narrative too cursed to ignore.`,
    ),
    makePost(rng, state.tick, "bot", 1),
  ]);
  pushMarketPoint(state);

  return state;
};

const activeChainPower = (chains: MomentumChain[]) =>
  chains.reduce(
    (totals, chain) => ({
      hypeVelocity: totals.hypeVelocity + chain.hypeVelocity * chain.intensity,
      liquidityMultiplier: totals.liquidityMultiplier + (chain.liquidityMultiplier - 1) * chain.intensity,
      heatAcceleration: totals.heatAcceleration + chain.heatAcceleration * chain.intensity,
      volatilityAcceleration: totals.volatilityAcceleration + chain.volatilityAcceleration * chain.intensity,
    }),
    { hypeVelocity: 0, liquidityMultiplier: 1, heatAcceleration: 0, volatilityAcceleration: 0 },
  );

const synergyMultiplierFor = (chains: MomentumChain[], actionId: ActionId) =>
  chains.reduce((multiplier, chain) => multiplier * (chain.synergy[actionId] ?? 1), 1);

const addMomentumChain = (state: GameState, actionId: ActionId, tick: number, multiplier: number) => {
  const action = actionDefinitions.find((candidate) => candidate.id === actionId);
  if (!action) {
    return;
  }

  const chain: MomentumChain = {
    id: `${actionId}-${tick}-${state.stats.actionsTaken}`,
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

  state.momentumChains = [chain, ...state.momentumChains.filter((candidate) => candidate.source !== actionId)].slice(0, 5);
};

export const performAction = (state: GameState, actionId: ActionId): GameState => {
  const next = cloneState(state);
  const coin = next.coin;
  const action = actionDefinitions.find((candidate) => candidate.id === actionId);

  if (!coin || next.phase !== "running" || !action || next.cash < action.cost || (next.cooldowns[actionId] ?? 0) > 0) {
    return state;
  }

  const rng = createRng(next.rngState);
  const chainMultiplier = synergyMultiplierFor(next.momentumChains, actionId);
  const effects = {
    ...action.effects,
    hype: action.effects.hype * chainMultiplier,
    attention: action.effects.attention * chainMultiplier,
    liquidity: action.effects.liquidity * chainMultiplier,
    heat: action.effects.heat * (0.85 + chainMultiplier * 0.28),
    volatility: action.effects.volatility * (0.9 + chainMultiplier * 0.22),
    narrativeStability: action.effects.narrativeStability * (0.85 + chainMultiplier * 0.2),
  };

  next.cash -= action.cost;
  next.stats.actionsTaken += 1;
  next.cooldowns[actionId] = action.cooldown;
  applyCoinEffects(coin, effects);
  addMomentumChain(next, actionId, next.tick, chainMultiplier);

  const boostedInfluencer = actionId === "sponsorInfluencer" ? rng.pick(next.influencers) : null;
  if (boostedInfluencer) {
    boostedInfluencer.alignment = clamp(boostedInfluencer.alignment + rng.float(8, 16), 0, 100);
    boostedInfluencer.volatility = clamp(boostedInfluencer.volatility + rng.float(2, 8), 0, 100);
  }

  const posts = [
    makePost(
      rng,
      next.tick,
      actionId === "manufactureOutrage" ? "unhinged" : "bullish",
      effects.hype,
      "@operator",
      `${action.command}${chainMultiplier > 1.05 ? ` // combo x${chainMultiplier.toFixed(2)}` : ""}`,
    ),
  ];

  if (rng.chance(0.32 + coin.heat / 220)) {
    posts.push(makePost(rng, next.tick, "bearish", -4));
  }

  applyFeedEffects(next, posts);
  addFeed(next, posts);
  addTerminal(next, `${action.label.toLowerCase()} executed. attention up, blast radius expanding.`);
  pushMarketPoint(next);
  next.rngState = rng.state;

  return next;
};

const spawnCrisis = (state: GameState, rng: Rng): Crisis | undefined => {
  const coin = state.coin;
  if (!coin || state.crises.length >= MAX_ACTIVE_CRISES) {
    return undefined;
  }

  const chainPressure = state.momentumChains.reduce((sum, chain) => sum + chain.intensity, 0);
  const instability = (100 - coin.narrativeStability) / 100;
  const crisisChance =
    0.05 + coin.heat / 240 + coin.volatility / 460 + Math.max(0, coin.hype - coin.trust) / 320 + chainPressure * 0.035 + instability * 0.12;
  if (!rng.chance(crisisChance)) {
    return undefined;
  }

  const typePool: CrisisId[] =
    coin.heat > 60 || coin.narrativeStability < 35
      ? ["regulatorPing", "outrageBackfire", "fakeHack", "rogueInfluencer"]
      : ["rogueInfluencer", "whaleDump", "fakeHack"];
  const type = rng.pick(typePool);
  return makeCrisis(type, `crisis-${state.tick}-${rng.int(100, 999)}`, state.tick, rng.float(18, 42) + chainPressure * 4 + instability * 16);
};

const escalateActiveCrises = (state: GameState, rng: Rng) => {
  const coin = state.coin;
  if (!coin || state.crises.length === 0) {
    return;
  }

  const chainPressure = state.momentumChains.reduce((sum, chain) => sum + chain.intensity, 0);
  state.crises = state.crises.map((crisis) => {
    const severityGain = 0.8 + coin.heat / 110 + Math.max(0, 45 - coin.narrativeStability) / 65 + chainPressure * 0.22;
    const severity = clamp(crisis.severity + severityGain, 8, 95);

    if (severity > crisis.severity + 2.4 && rng.chance(0.18 + state.crises.length * 0.04)) {
      addTerminal(state, `${crisis.title.toLowerCase()} escalated. ignoring it is now a position.`);
    }

    return { ...crisis, severity };
  });
};

const resolveExpiredCrises = (state: GameState, rng: Rng) => {
  const coin = state.coin;
  if (!coin || state.crises.length === 0) {
    return;
  }

  const expired = state.crises.filter((crisis) => state.tick >= crisis.expiresAt);
  if (expired.length === 0) {
    return;
  }

  for (const crisis of expired) {
    applyCoinEffects(coin, {
      hype: -crisis.severity * 0.18,
      trust: -crisis.severity * 0.28,
      heat: crisis.severity * 0.2,
      attention: crisis.severity * 0.12,
      liquidity: -crisis.severity * rng.float(28, 52),
      volatility: crisis.severity * 0.16,
      narrativeStability: -crisis.severity * 0.24,
      momentum: -crisis.severity * 0.12,
    });
    addFeed(state, [makePost(rng, state.tick, "bearish", -crisis.severity, "@panicDesk", `${crisis.title} metastasized before you controlled it.`)]);
    addTerminal(state, `${crisis.title.toLowerCase()} timed out. the timeline chose violence.`);
  }

  state.crises = state.crises.filter((crisis) => state.tick < crisis.expiresAt);
};

export const advanceTick = (state: GameState): TickResult => {
  const next = cloneState(state);
  const coin = next.coin;

  if (!coin || next.phase !== "running") {
    return { state: next, posts: [] };
  }

  const rng = createRng(next.rngState);
  next.tick += 1;
  next.momentumChains = next.momentumChains.filter((chain) => chain.expiresAt > next.tick);

  for (const key of Object.keys(next.cooldowns) as ActionId[]) {
    next.cooldowns[key] = Math.max(0, (next.cooldowns[key] ?? 0) - 1);
  }

  escalateActiveCrises(next, rng);
  resolveExpiredCrises(next, rng);

  const crisisDrag = next.crises.reduce((sum, crisis) => sum + crisis.severity, 0);
  const chainPower = activeChainPower(next.momentumChains);
  const narrativePenalty = (100 - coin.narrativeStability) * 0.22;
  const beliefPressure =
    coin.hype * 0.5 +
    coin.attention * 0.3 +
    coin.trust * 0.18 +
    coin.momentum * 0.34 +
    chainPower.hypeVelocity * 1.8 -
    coin.heat * 0.42 -
    crisisDrag * 0.24 -
    narrativePenalty;
  const liquidityFlow = beliefPressure * rng.float(12, 28) * chainPower.liquidityMultiplier + rng.float(-650, 900);
  const whaleTrade = rng.chance(0.08 + coin.volatility / 700 + Math.max(0, coin.momentum) / 900)
    ? rng.pick([-1, 1]) * rng.float(0.08, 0.34 + coin.volatility / 260)
    : 0;
  const panicSelloff = coin.trust < 30 && rng.chance(0.12 + (30 - coin.trust) / 120) ? -rng.float(0.18, 0.55) : 0;
  const euphoriaPump = coin.momentum > 55 && rng.chance(0.1 + coin.attention / 650) ? rng.float(0.16, 0.48) : 0;
  const volatilityShock = rng.float(-coin.volatility, coin.volatility) / 92;
  const priceMove = clamp((beliefPressure - 34) / 105 + volatilityShock - coin.heat / 430 + whaleTrade + panicSelloff + euphoriaPump, -0.68, 1.1);
  const absoluteMove = Math.abs(priceMove);

  coin.liquidity = Math.max(0, coin.liquidity + liquidityFlow);
  coin.price = Math.max(0.00001, coin.price * (1 + priceMove));
  coin.marketCap = coin.price * SUPPLY;
  coin.volume = Math.max(80, coin.liquidity * (0.05 + absoluteMove * rng.float(0.75, 1.6) + coin.attention / 900));
  coin.priceVelocity = priceMove;
  coin.hype = clamp(coin.hype * 0.9 + coin.attention * 0.075 + chainPower.hypeVelocity + rng.float(-3.5, 4.5), 0, 100);
  coin.attention = clamp(coin.attention * 0.875 + coin.hype * 0.055 + absoluteMove * 10 + rng.float(-2, 5.5), 0, 100);
  coin.trust = clamp(
    coin.trust - coin.heat * 0.036 - Math.max(0, coin.volatility - 55) * 0.04 - Math.max(0, absoluteMove - 0.28) * 8 + rng.float(-2.2, 2.4),
    0,
    100,
  );
  coin.heat = clamp(coin.heat * 0.95 + Math.max(0, coin.hype - 58) * 0.04 + chainPower.heatAcceleration + rng.float(-1.3, 2.4), 0, 100);
  coin.volatility = clamp(coin.volatility * 0.9 + absoluteMove * 54 + chainPower.volatilityAcceleration + rng.float(-2.4, 3.2), 5, 100);
  coin.momentum = clamp(coin.momentum * 0.84 + priceMove * 44 + chainPower.hypeVelocity * 1.7 + rng.float(-5, 5), -100, 100);
  coin.narrativeStability = clamp(
    coin.narrativeStability - coin.heat * 0.026 - next.crises.length * 1.2 - Math.max(0, coin.hype - coin.trust) * 0.018 + rng.float(-1.3, 1.8),
    0,
    100,
  );
  next.cash += Math.max(0, coin.liquidity * 0.006 * (coin.trust / 100));

  if (whaleTrade !== 0) {
    const whalePosts = [
      makePost(
        rng,
        next.tick,
        whaleTrade > 0 ? "bullish" : "bearish",
        whaleTrade * 40,
        "@whaleScanner",
        whaleTrade > 0 ? "Whale wallet aped so hard the chart flinched." : "Whale wallet dumped into retail optimism.",
      ),
    ];
    applyFeedEffects(next, whalePosts);
    addFeed(next, whalePosts);
  }

  for (const influencer of next.influencers) {
    if (influencer.status === "cancelled") {
      continue;
    }

    influencer.alignment = clamp(influencer.alignment - coin.heat * 0.012 + rng.float(-2.5, 2), 0, 100);
    if (influencer.status === "loyal" && influencer.alignment < 24 && rng.chance(influencer.volatility / 240)) {
      influencer.status = "rogue";
      applyCoinEffects(coin, { hype: 7, heat: 8, trust: -9, volatility: 7, narrativeStability: -8, momentum: -4 });
      const roguePosts = [
        makePost(rng, next.tick, "unhinged", 8, influencer.handle, "I was never paid enough to pretend this was sane."),
      ];
      applyFeedEffects(next, roguePosts);
      addFeed(next, roguePosts);
    }
  }

  const sentiment = deriveSentiment(coin, rng);
  const posts = [
    makePost(rng, next.tick, sentiment, beliefPressure),
    makePost(rng, next.tick, rng.chance(0.25) ? "unhinged" : sentiment, beliefPressure * 0.6),
  ];
  applyFeedEffects(next, posts);
  addFeed(next, posts);

  const crisis = spawnCrisis(next, rng);
  if (crisis) {
    next.crises = [...next.crises, crisis];
    addTerminal(next, `crisis spawned: ${crisis.title}. respond before it becomes consensus.`);
    const crisisPosts = [makePost(rng, next.tick, "regulatory", -crisis.severity, "@riskDaemon", crisis.body)];
    applyFeedEffects(next, crisisPosts);
    addFeed(next, crisisPosts);
  }

  if (coin.trust <= 1 || coin.liquidity <= 150 || coin.narrativeStability <= 1 || (coin.heat >= 98 && rng.chance(0.34))) {
    next.phase = "collapsed";
    addTerminal(next, "collapse detected. belief failed its margin call.");
    addFeed(next, [
      makePost(rng, next.tick, "bearish", -100, "@postmortem", `$${coin.ticker} is now a cautionary screenshot.`),
    ]);
  }

  pushMarketPoint(next);
  next.rngState = rng.state;

  return { state: next, posts, crisis };
};

export const resolveCrisis = (state: GameState, crisisId: string, choiceId: string): GameState => {
  const next = cloneState(state);
  const coin = next.coin;
  const crisis = next.crises.find((candidate) => candidate.id === crisisId);
  const choice = crisis?.choices.find((candidate) => candidate.id === choiceId);

  if (!coin || !crisis || !choice || next.phase !== "running") {
    return state;
  }

  const rng = createRng(next.rngState);
  applyCoinEffects(coin, choice.effects);
  next.cash = Math.max(0, next.cash + (choice.effects.cash ?? 0));
  next.crises = next.crises.filter((candidate) => candidate.id !== crisisId);
  next.stats.crisesSurvived += 1;
  addTerminal(next, `${crisis.title.toLowerCase()} contained via "${choice.label.toLowerCase()}". probably.`);
  addFeed(next, [
    makePost(rng, next.tick, choice.effects.trust && choice.effects.trust > 0 ? "bullish" : "unhinged", crisis.severity, "@crisisPR", choice.description),
  ]);
  pushMarketPoint(next);
  next.rngState = rng.state;

  return next;
};

export const rugPull = (state: GameState): GameState => {
  const next = cloneState(state);
  const coin = next.coin;
  if (!coin || next.phase !== "running") {
    return state;
  }

  const rng = createRng(next.rngState);
  const extractionRate = clamp(0.2 + coin.hype / 250 + coin.trust / 420 - coin.heat / 310, 0.08, 0.72);
  const extracted = coin.liquidity * extractionRate;
  next.cash += extracted;
  next.phase = "cashedOut";
  next.stats.rugPulls += 1;
  coin.liquidity = Math.max(0, coin.liquidity - extracted);
  coin.trust = 0;
  coin.heat = 100;
  coin.hype = clamp(coin.hype + 12, 0, 100);
  coin.volatility = 100;
  coin.momentum = -100;
  coin.narrativeStability = 0;
  addTerminal(next, `rug pull executed. ${Math.round(extractionRate * 100)}% liquidity extracted before sirens.`);
  addFeed(next, [
    makePost(rng, next.tick, "bearish", -90, "@exitLiquidity", "The founder wallet just discovered teleportation."),
    makePost(rng, next.tick, "unhinged", 40, "@bagholderUnion", "THIS IS FINE. THE ROADMAP INCLUDED BETRAYAL."),
  ]);
  pushMarketPoint(next);
  next.rngState = rng.state;

  return next;
};

export const pivotNarrative = (state: GameState): GameState => {
  const next = cloneState(state);
  const coin = next.coin;
  if (!coin || next.phase !== "running" || next.cash < 1_500) {
    return state;
  }

  const rng = createRng(next.rngState);
  const oldNarrative = coin.narrative;
  coin.narrative = rng.pick(narrativeSeeds);
  next.cash -= 1_500;
  applyCoinEffects(coin, { hype: 15, attention: 16, trust: -12, heat: 8, volatility: 12, narrativeStability: -18, momentum: 12 });
  addTerminal(next, `pivoted from "${oldNarrative}" to "${coin.narrative}". nobody remembers yesterday.`);
  addFeed(next, [makePost(rng, next.tick, "unhinged", 16, "@narrativeDesk", "We are so early the premise changed twice.")]);
  pushMarketPoint(next);
  next.rngState = rng.state;

  return next;
};
