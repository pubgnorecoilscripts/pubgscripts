import type { Rng } from "./rng";
import { clamp } from "./rng";
import type { CoinState, Sentiment, SocialPost } from "./types";

export type FactionId =
  | "retailSwarm"
  | "aiCultists"
  | "regulators"
  | "whaleSyndicate"
  | "doomers"
  | "conspiracyAccounts"
  | "trendChasers";

export type FactionState = {
  id: FactionId;
  label: string;
  power: number;
  sentiment: number;
  activity: number;
};

export type FactionDef = {
  id: FactionId;
  label: string;
  initialPower: number;
  sentimentBias: Sentiment;
  postAuthors: string[];
  postTemplates: string[];
  reactionThresholds: {
    metric: keyof CoinState;
    above?: number;
    below?: number;
    effect: "amplify" | "suppress" | "panic" | "frenzy";
  }[];
  tickEffects: (faction: FactionState, coin: CoinState) => Partial<CoinState>;
};

export const factionDefs: FactionDef[] = [
  {
    id: "retailSwarm",
    label: "Retail Swarm",
    initialPower: 60,
    sentimentBias: "bullish",
    postAuthors: ["@degen_carl", "@first_buy_no_research", "@hopium_enjoyer", "@lambo_timeline"],
    postTemplates: [
      "Just aped my rent money. Can't go wrong if you never think about it.",
      "The chart looks like my heart rate when I check my portfolio.",
      "New ATH incoming. Source: I desperately need this.",
      "Diamond hands is just poverty with conviction.",
      "My wife's boyfriend said this was a good entry.",
    ],
    reactionThresholds: [
      { metric: "hype", above: 65, effect: "frenzy" },
      { metric: "trust", below: 25, effect: "panic" },
      { metric: "momentum", above: 50, effect: "amplify" },
    ],
    tickEffects: (faction, coin) => ({
      hype: faction.power > 50 && coin.hype > 40 ? 0.3 : -0.1,
      liquidity: faction.sentiment > 60 ? faction.power * 1.5 : -faction.power * 0.5,
    }),
  },
  {
    id: "aiCultists",
    label: "AI Cultists",
    initialPower: 35,
    sentimentBias: "unhinged",
    postAuthors: ["@gptworship", "@sentient_slop", "@neural_prophet", "@alignment_cope"],
    postTemplates: [
      "The token IS the intelligence. You wouldn't understand.",
      "My fine-tuned model says this is the singularity of liquidity.",
      "The AI said buy. I don't question the AI. The AI questions me.",
      "Consciousness is just market cap with extra steps.",
      "The model hallucinated a roadmap and it's better than the real one.",
    ],
    reactionThresholds: [
      { metric: "narrativeStability", below: 40, effect: "frenzy" },
      { metric: "volatility", above: 60, effect: "amplify" },
      { metric: "hype", above: 70, effect: "amplify" },
    ],
    tickEffects: (faction, coin) => ({
      narrativeStability: faction.power > 40 ? -0.2 : 0,
      hype: faction.activity > 60 ? 0.5 : 0,
      volatility: faction.power > 50 ? 0.2 : 0,
      attention: coin.narrativeStability < 35 ? 0.6 : 0,
    }),
  },
  {
    id: "regulators",
    label: "Regulators",
    initialPower: 20,
    sentimentBias: "regulatory",
    postAuthors: ["@sec_intern_anon", "@compliance_ghost", "@finra_enjoyer", "@enforcement_watch"],
    postTemplates: [
      "This is textbook securities fraud but with emojis.",
      "Filed a complaint. They sent back a meme. Escalating.",
      "The enforcement division found the subreddit. Meetings are being scheduled.",
      "Someone said 'investment' in the Discord. That's all we needed.",
      "Jurisdictional clarity incoming. It will not be fun.",
    ],
    reactionThresholds: [
      { metric: "heat", above: 60, effect: "amplify" },
      { metric: "trust", below: 30, effect: "amplify" },
      { metric: "attention", above: 70, effect: "frenzy" },
    ],
    tickEffects: (faction, coin) => ({
      heat: faction.power > 40 ? 0.3 : 0.05,
      trust: coin.heat > 55 ? -0.2 : 0,
      narrativeStability: faction.activity > 50 ? -0.15 : 0,
    }),
  },
  {
    id: "whaleSyndicate",
    label: "Whale Syndicate",
    initialPower: 45,
    sentimentBias: "bearish",
    postAuthors: ["@deep_wallet", "@silent_accumulator", "@exit_architect", "@liquidity_predator"],
    postTemplates: [
      "Interesting liquidity depth here. Would be a shame if someone tested it.",
      "My position is larger than your community's hope.",
      "Retail provides the exit liquidity. We provide the exit.",
      "The spread is an invitation written in blood.",
      "Consolidating. You'll know when I'm done because the chart will remember.",
    ],
    reactionThresholds: [
      { metric: "liquidity", above: 4000, effect: "amplify" },
      { metric: "volatility", above: 55, effect: "frenzy" },
      { metric: "momentum", above: 40, effect: "suppress" },
    ],
    tickEffects: (faction, coin) => ({
      volatility: faction.activity > 55 ? 0.4 : 0.1,
      liquidity: faction.sentiment < 40 ? -faction.power * 1.5 : faction.power * 0.8,
      momentum: faction.activity > 60 && coin.momentum > 30 ? -0.8 : 0,
    }),
  },
  {
    id: "doomers",
    label: "Doomers",
    initialPower: 30,
    sentimentBias: "bearish",
    postAuthors: ["@its_over_anon", "@zero_timeline", "@rugwatch_daily", "@exit_liquidity_club"],
    postTemplates: [
      "I've seen this exact chart before. It ended in screenshots and therapy.",
      "The devs are 'cooking' (burning evidence).",
      "Every pump is just a more expensive rug pull.",
      "Narrator: it was, in fact, over.",
      "The roadmap has more fiction than the Harry Potter series.",
    ],
    reactionThresholds: [
      { metric: "trust", below: 35, effect: "amplify" },
      { metric: "heat", above: 50, effect: "amplify" },
      { metric: "hype", above: 75, effect: "suppress" },
    ],
    tickEffects: (faction, coin) => ({
      trust: faction.power > 40 ? -0.15 : 0,
      hype: coin.trust < 30 ? -0.3 : 0,
      momentum: faction.activity > 50 ? -0.2 : 0,
    }),
  },
  {
    id: "conspiracyAccounts",
    label: "Conspiracy Accounts",
    initialPower: 25,
    sentimentBias: "unhinged",
    postAuthors: ["@hidden_hand", "@thread_puller", "@pattern_seer", "@deep_lore_anon"],
    postTemplates: [
      "The wallet connections form a pentagram if you map them correctly.",
      "This was planned since 2019. I have the screenshots. No I won't share them.",
      "The 'hack' was an inside job to shake paper hands before the real pump.",
      "Follow the wallets. They all lead to the same DAO. The DAO leads to a senator.",
      "The conspiracy isn't the coin. The coin IS the conspiracy.",
    ],
    reactionThresholds: [
      { metric: "narrativeStability", below: 35, effect: "frenzy" },
      { metric: "heat", above: 55, effect: "amplify" },
      { metric: "volatility", above: 50, effect: "amplify" },
    ],
    tickEffects: (faction, coin) => ({
      attention: faction.activity > 45 ? 0.4 : 0,
      narrativeStability: faction.power > 35 ? -0.25 : 0,
      hype: coin.narrativeStability < 30 ? 0.2 : 0,
      volatility: faction.activity > 55 ? 0.2 : 0,
    }),
  },
  {
    id: "trendChasers",
    label: "Trend Chasers",
    initialPower: 50,
    sentimentBias: "bullish",
    postAuthors: ["@alpha_leaks", "@narrative_surfer", "@momentum_slave", "@fomo_index"],
    postTemplates: [
      "Everyone I follow just followed this coin. That's my DD.",
      "The narrative is rotating here. I can feel it in the engagement metrics.",
      "If it's trending it's too late. Unless it's still trending. Then it's early.",
      "My strategy is other people's conviction. Works until it doesn't.",
      "The meta shifted. I'm already positioned. (I bought 30 seconds ago.)",
    ],
    reactionThresholds: [
      { metric: "momentum", above: 45, effect: "amplify" },
      { metric: "attention", above: 60, effect: "frenzy" },
      { metric: "momentum", below: -20, effect: "panic" },
    ],
    tickEffects: (faction, coin) => ({
      hype: coin.momentum > 30 ? faction.power * 0.008 : -0.15,
      momentum: faction.activity > 55 ? (coin.momentum > 0 ? 0.4 : -0.4) : 0,
      liquidity: coin.momentum > 40 ? faction.power * 1.0 : 0,
    }),
  },
];

export const createFactionStates = (rng: Rng): FactionState[] =>
  factionDefs.map((def) => ({
    id: def.id,
    label: def.label,
    power: def.initialPower * 0.5 + rng.float(-3, 3),
    sentiment: 50 + rng.float(-5, 5),
    activity: 10 + rng.float(-2, 5),
  }));

export const tickFactions = (
  factions: FactionState[],
  coin: CoinState,
  rng: Rng,
): { factions: FactionState[]; effects: Partial<CoinState>; posts: Omit<SocialPost, "id" | "tick">[] } => {
  const totalEffects: Partial<CoinState> = {};
  const posts: Omit<SocialPost, "id" | "tick">[] = [];

  const updatedFactions = factions.map((faction) => {
    const def = factionDefs.find((d) => d.id === faction.id);
    if (!def) return faction;

    const next = { ...faction };

    for (const threshold of def.reactionThresholds) {
      const metricVal = coin[threshold.metric] as number;
      if (threshold.above !== undefined && metricVal > threshold.above) {
        if (threshold.effect === "amplify") next.activity = clamp(next.activity + 4, 0, 100);
        if (threshold.effect === "frenzy") next.activity = clamp(next.activity + 8, 0, 100);
        if (threshold.effect === "suppress") next.activity = clamp(next.activity - 3, 0, 100);
        if (threshold.effect === "panic") next.sentiment = clamp(next.sentiment - 6, 0, 100);
      }
      if (threshold.below !== undefined && metricVal < threshold.below) {
        if (threshold.effect === "amplify") next.activity = clamp(next.activity + 4, 0, 100);
        if (threshold.effect === "frenzy") next.activity = clamp(next.activity + 8, 0, 100);
        if (threshold.effect === "suppress") next.activity = clamp(next.activity - 3, 0, 100);
        if (threshold.effect === "panic") next.sentiment = clamp(next.sentiment - 6, 0, 100);
      }
    }

    next.activity = clamp(next.activity * 0.92 + rng.float(-2, 3), 0, 100);
    next.sentiment = clamp(next.sentiment + rng.float(-3, 3), 0, 100);
    next.power = clamp(next.power + rng.float(-1.5, 1.5) + (next.activity > 60 ? 0.8 : -0.3), 5, 100);

    if (next.activity > 55) {
      const fxRaw = def.tickEffects(next, coin);
      const scale = (next.power / 100) * (next.activity / 100);
      const fx = fxRaw as Record<string, number | undefined>;
      const acc = totalEffects as Record<string, number | undefined>;
      for (const key of Object.keys(fx)) {
        const v = fx[key];
        if (v) {
          acc[key] = (acc[key] ?? 0) + v * scale;
        }
      }
    }

    if (next.activity > 55 && rng.chance(0.15 + next.activity / 400)) {
      posts.push({
        author: rng.pick(def.postAuthors),
        body: rng.pick(def.postTemplates),
        sentiment: def.sentimentBias,
        impact: next.power * 0.12,
        effects: {},
      });
    }

    return next;
  });

  return { factions: updatedFactions, effects: totalEffects, posts };
};
