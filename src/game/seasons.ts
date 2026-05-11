import type { Rng } from "./rng";
import type { Season, SeasonId } from "./types";

export const seasonDefs: Record<SeasonId, Omit<Season, "ticksRemaining">> = {
  aiBubble: {
    id: "aiBubble",
    label: "AI BUBBLE ERA",
    description: "Every narrative with 'AI' in it pumps 3x harder. Reality optional.",
    effects: {
      volatilityMod: 1.4,
      hypeDecayMod: 0.7,
      trustFloor: 0,
      crisisRateMod: 0.8,
      factionPowerMods: { aiCultists: 1.6, trendChasers: 1.3 },
      narrativeBonus: ["AI", "neural", "sentient", "consciousness"],
    },
  },
  antiTechPanic: {
    id: "antiTechPanic",
    label: "ANTI-TECH PANIC",
    description: "Public trust in technology cratered. Heat rises faster. Regulators are hungry.",
    effects: {
      volatilityMod: 1.2,
      hypeDecayMod: 1.3,
      trustFloor: 0,
      crisisRateMod: 1.5,
      factionPowerMods: { regulators: 1.8, doomers: 1.4, conspiracyAccounts: 1.2 },
      narrativeBonus: ["privacy", "decentralized", "anonymous"],
    },
  },
  memeRenaissance: {
    id: "memeRenaissance",
    label: "MEME RENAISSANCE",
    description: "Irony is currency. The stupider the narrative, the more it pumps.",
    effects: {
      volatilityMod: 1.6,
      hypeDecayMod: 0.5,
      trustFloor: 0,
      crisisRateMod: 0.9,
      factionPowerMods: { retailSwarm: 1.5, trendChasers: 1.4, aiCultists: 0.7 },
      narrativeBonus: ["meme", "vibe", "slop", "goblin", "cursed"],
    },
  },
  doomEconomy: {
    id: "doomEconomy",
    label: "DOOM ECONOMY",
    description: "Markets are bleeding. Only short-sellers and doomers thrive.",
    effects: {
      volatilityMod: 1.8,
      hypeDecayMod: 1.6,
      trustFloor: 0,
      crisisRateMod: 1.4,
      factionPowerMods: { doomers: 2.0, whaleSyndicate: 1.5, retailSwarm: 0.5 },
      narrativeBonus: ["doom", "collapse", "zero", "exit"],
    },
  },
  regulatoryPurge: {
    id: "regulatoryPurge",
    label: "REGULATORY PURGE",
    description: "Agencies are hunting. Heat accumulates 2x faster. Trust is everything.",
    effects: {
      volatilityMod: 0.8,
      hypeDecayMod: 1.1,
      trustFloor: 15,
      crisisRateMod: 1.8,
      factionPowerMods: { regulators: 2.2, doomers: 1.2, retailSwarm: 0.6, conspiracyAccounts: 0.8 },
      narrativeBonus: ["compliance", "transparent", "regulated"],
    },
  },
  conspiracyBoom: {
    id: "conspiracyBoom",
    label: "CONSPIRACY BOOM",
    description: "Nobody trusts anything. Every narrative spawns counter-narratives.",
    effects: {
      volatilityMod: 1.5,
      hypeDecayMod: 0.8,
      trustFloor: 0,
      crisisRateMod: 1.2,
      factionPowerMods: { conspiracyAccounts: 2.0, aiCultists: 1.3, regulators: 0.6 },
      narrativeBonus: ["conspiracy", "hidden", "secret", "shadow"],
    },
  },
  quantumMania: {
    id: "quantumMania",
    label: "QUANTUM FINANCE MANIA",
    description: "Markets move on vibes and quantum probability. Nothing makes sense. Everything pumps.",
    effects: {
      volatilityMod: 2.0,
      hypeDecayMod: 0.4,
      trustFloor: 0,
      crisisRateMod: 1.0,
      factionPowerMods: { trendChasers: 1.6, retailSwarm: 1.4, whaleSyndicate: 1.3 },
      narrativeBonus: ["quantum", "entangled", "probability", "multiverse"],
    },
  },
};

const SEASON_DURATION_MIN = 80;
const SEASON_DURATION_MAX = 160;

export const createSeason = (rng: Rng, exclude?: SeasonId): Season => {
  const ids = Object.keys(seasonDefs) as SeasonId[];
  const pool = exclude ? ids.filter((id) => id !== exclude) : ids;
  const id = rng.pick(pool);
  const def = seasonDefs[id];
  return {
    ...def,
    ticksRemaining: rng.int(SEASON_DURATION_MIN, SEASON_DURATION_MAX),
  };
};

export const tickSeason = (season: Season, rng: Rng): { season: Season; changed: boolean } => {
  const next = { ...season, effects: { ...season.effects } };
  next.ticksRemaining -= 1;

  if (next.ticksRemaining <= 0) {
    return { season: createSeason(rng, season.id), changed: true };
  }

  return { season: next, changed: false };
};
