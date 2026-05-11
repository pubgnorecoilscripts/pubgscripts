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
  Sentiment,
  SocialPost,
  TickResult,
} from "./types";

const SUPPLY = 1_000_000_000;
const MAX_FEED_ITEMS = 42;
const MAX_MARKET_POINTS = 80;

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
    hype: state.coin.hype,
    trust: state.coin.trust,
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
): SocialPost => ({
  id: `${tick}-${sentiment}-${rng.int(1000, 9999)}`,
  tick,
  author: author ?? rng.pick(["@anon", "@bagwatcher", "@terminallyLong", "@doomscroll_cfo", "@chain_janitor"]),
  body: body ?? rng.pick(postFragments[sentiment]),
  sentiment,
  impact,
});

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
    hype: rng.float(18, 28),
    trust: rng.float(48, 62),
    heat: rng.float(5, 12),
    attention: rng.float(20, 34),
    volatility: rng.float(18, 28),
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

export const performAction = (state: GameState, actionId: ActionId): GameState => {
  const next = cloneState(state);
  const coin = next.coin;
  const action = actionDefinitions.find((candidate) => candidate.id === actionId);

  if (!coin || next.phase !== "running" || !action || next.cash < action.cost || (next.cooldowns[actionId] ?? 0) > 0) {
    return state;
  }

  const rng = createRng(next.rngState);
  next.cash -= action.cost;
  next.stats.actionsTaken += 1;
  next.cooldowns[actionId] = action.cooldown;
  applyCoinEffects(coin, action.effects);

  const boostedInfluencer = actionId === "sponsorInfluencer" ? rng.pick(next.influencers) : null;
  if (boostedInfluencer) {
    boostedInfluencer.alignment = clamp(boostedInfluencer.alignment + rng.float(8, 16), 0, 100);
    boostedInfluencer.volatility = clamp(boostedInfluencer.volatility + rng.float(2, 8), 0, 100);
  }

  const posts = [
    makePost(rng, next.tick, actionId === "manufactureOutrage" ? "unhinged" : "bullish", action.effects.hype, "@operator", action.command),
  ];

  if (rng.chance(0.32 + coin.heat / 220)) {
    posts.push(makePost(rng, next.tick, "bearish", -4));
  }

  addFeed(next, posts);
  addTerminal(next, `${action.label.toLowerCase()} executed. attention up, blast radius expanding.`);
  pushMarketPoint(next);
  next.rngState = rng.state;

  return next;
};

const spawnCrisis = (state: GameState, rng: Rng): Crisis | undefined => {
  const coin = state.coin;
  if (!coin || state.crises.length >= 2) {
    return undefined;
  }

  const crisisChance = 0.05 + coin.heat / 280 + coin.volatility / 520 + Math.max(0, coin.hype - coin.trust) / 360;
  if (!rng.chance(crisisChance)) {
    return undefined;
  }

  const typePool: CrisisId[] = coin.heat > 60 ? ["regulatorPing", "outrageBackfire", "fakeHack"] : ["rogueInfluencer", "whaleDump", "fakeHack"];
  const type = rng.pick(typePool);
  return makeCrisis(type, `crisis-${state.tick}-${rng.int(100, 999)}`, state.tick, rng.float(18, 42));
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

  for (const key of Object.keys(next.cooldowns) as ActionId[]) {
    next.cooldowns[key] = Math.max(0, (next.cooldowns[key] ?? 0) - 1);
  }

  resolveExpiredCrises(next, rng);

  const crisisDrag = next.crises.reduce((sum, crisis) => sum + crisis.severity, 0);
  const beliefPressure = coin.hype * 0.55 + coin.attention * 0.28 + coin.trust * 0.2 - coin.heat * 0.46 - crisisDrag * 0.22;
  const liquidityFlow = beliefPressure * rng.float(10, 22) + rng.float(-450, 650);
  const volatilityShock = rng.float(-coin.volatility, coin.volatility) / 100;
  const priceMove = clamp((beliefPressure - 38) / 120 + volatilityShock - coin.heat / 420, -0.42, 0.7);

  coin.liquidity = Math.max(0, coin.liquidity + liquidityFlow);
  coin.price = Math.max(0.00001, coin.price * (1 + priceMove));
  coin.marketCap = coin.price * SUPPLY;
  coin.hype = clamp(coin.hype * 0.91 + coin.attention * 0.08 + rng.float(-3, 4), 0, 100);
  coin.attention = clamp(coin.attention * 0.88 + coin.hype * 0.05 + rng.float(-2, 5), 0, 100);
  coin.trust = clamp(coin.trust - coin.heat * 0.035 - Math.max(0, coin.volatility - 55) * 0.035 + rng.float(-2, 2.4), 0, 100);
  coin.heat = clamp(coin.heat * 0.95 + Math.max(0, coin.hype - 60) * 0.035 + rng.float(-1.3, 2.2), 0, 100);
  coin.volatility = clamp(coin.volatility * 0.92 + Math.abs(priceMove) * 45 + rng.float(-2, 3), 5, 100);
  next.cash += Math.max(0, coin.liquidity * 0.006 * (coin.trust / 100));

  for (const influencer of next.influencers) {
    if (influencer.status === "cancelled") {
      continue;
    }

    influencer.alignment = clamp(influencer.alignment - coin.heat * 0.012 + rng.float(-2.5, 2), 0, 100);
    if (influencer.status === "loyal" && influencer.alignment < 24 && rng.chance(influencer.volatility / 240)) {
      influencer.status = "rogue";
      applyCoinEffects(coin, { hype: 7, heat: 8, trust: -9, volatility: 7 });
      addFeed(next, [
        makePost(rng, next.tick, "unhinged", 8, influencer.handle, "I was never paid enough to pretend this was sane."),
      ]);
    }
  }

  const sentiment = deriveSentiment(coin, rng);
  const posts = [
    makePost(rng, next.tick, sentiment, beliefPressure),
    makePost(rng, next.tick, rng.chance(0.25) ? "unhinged" : sentiment, beliefPressure * 0.6),
  ];
  addFeed(next, posts);

  const crisis = spawnCrisis(next, rng);
  if (crisis) {
    next.crises = [...next.crises, crisis];
    addTerminal(next, `crisis spawned: ${crisis.title}. respond before it becomes consensus.`);
    addFeed(next, [makePost(rng, next.tick, "regulatory", -crisis.severity, "@riskDaemon", crisis.body)]);
  }

  if (coin.trust <= 1 || coin.liquidity <= 150 || (coin.heat >= 98 && rng.chance(0.34))) {
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
  applyCoinEffects(coin, { hype: 15, attention: 16, trust: -12, heat: 8, volatility: 12 });
  addTerminal(next, `pivoted from "${oldNarrative}" to "${coin.narrative}". nobody remembers yesterday.`);
  addFeed(next, [makePost(rng, next.tick, "unhinged", 16, "@narrativeDesk", "We are so early the premise changed twice.")]);
  pushMarketPoint(next);
  next.rngState = rng.state;

  return next;
};
