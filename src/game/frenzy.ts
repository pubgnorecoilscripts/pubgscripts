import { clamp } from "./rng";
import type { ActionId } from "./types";

export type FrenzyState = {
  active: boolean;
  intensity: number;
  ticksRemaining: number;
  actionsInWindow: number;
  windowStart: number;
  comboActions: ActionId[];
  peakIntensity: number;
  totalFrenzies: number;
};

export type FrenzyConfig = {
  actionsToTrigger: number;
  windowTicks: number;
  baseDuration: number;
  intensityPerAction: number;
  maxIntensity: number;
  decayRate: number;
};

export const FRENZY_CONFIG: FrenzyConfig = {
  actionsToTrigger: 3,
  windowTicks: 10,
  baseDuration: 12,
  intensityPerAction: 0.3,
  maxIntensity: 2.5,
  decayRate: 0.06,
};

export const createFrenzyState = (): FrenzyState => ({
  active: false,
  intensity: 0,
  ticksRemaining: 0,
  actionsInWindow: 0,
  windowStart: 0,
  comboActions: [],
  peakIntensity: 0,
  totalFrenzies: 0,
});

export const frenzyMultipliers = (frenzy: FrenzyState) => {
  if (!frenzy.active) {
    return {
      hypeGain: 1,
      liquidityAccel: 1,
      feedVelocity: 1,
      volatilityAmp: 1,
      crisisSpawnRate: 1,
      heatAccel: 1,
      stabilityDrain: 1,
      tickSpeed: 1,
    };
  }

  const i = frenzy.intensity;
  return {
    hypeGain: 1 + i * 0.6,
    liquidityAccel: 1 + i * 0.45,
    feedVelocity: 1 + i * 0.8,
    volatilityAmp: 1 + i * 0.5,
    crisisSpawnRate: 1 + i * 0.4,
    heatAccel: 1 + i * 0.35,
    stabilityDrain: 1 + i * 0.55,
    tickSpeed: 1 + i * 0.3,
  };
};

export const recordFrenzyAction = (
  frenzy: FrenzyState,
  actionId: ActionId,
  currentTick: number,
): FrenzyState => {
  const next = { ...frenzy, comboActions: [...frenzy.comboActions] };

  if (next.active) {
    next.intensity = clamp(next.intensity + FRENZY_CONFIG.intensityPerAction, 0, FRENZY_CONFIG.maxIntensity);
    next.ticksRemaining = Math.min(next.ticksRemaining + 3, FRENZY_CONFIG.baseDuration + 8);
    next.peakIntensity = Math.max(next.peakIntensity, next.intensity);
    return next;
  }

  if (currentTick - next.windowStart > FRENZY_CONFIG.windowTicks) {
    next.actionsInWindow = 0;
    next.windowStart = currentTick;
    next.comboActions = [];
  }

  next.actionsInWindow += 1;
  next.comboActions.push(actionId);

  const uniqueActions = new Set(next.comboActions).size;
  const synergyBonus = uniqueActions >= 3 ? 1 : 0;

  if (next.actionsInWindow + synergyBonus >= FRENZY_CONFIG.actionsToTrigger) {
    next.active = true;
    next.intensity = 1.0 + uniqueActions * 0.15;
    next.ticksRemaining = FRENZY_CONFIG.baseDuration + uniqueActions * 2;
    next.peakIntensity = next.intensity;
    next.totalFrenzies += 1;
    next.actionsInWindow = 0;
    next.comboActions = [];
  }

  return next;
};

export const tickFrenzy = (frenzy: FrenzyState): FrenzyState => {
  if (!frenzy.active) return frenzy;

  const next = { ...frenzy };
  next.ticksRemaining -= 1;
  next.intensity = clamp(next.intensity - FRENZY_CONFIG.decayRate, 0, FRENZY_CONFIG.maxIntensity);

  if (next.ticksRemaining <= 0 || next.intensity <= 0.1) {
    next.active = false;
    next.intensity = 0;
    next.ticksRemaining = 0;
  }

  return next;
};
