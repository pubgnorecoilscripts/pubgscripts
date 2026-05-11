import { create } from "zustand";
import { actionDefinitions } from "../game/content";
import {
  advanceTick,
  crisisFreeze,
  createWorld,
  launchNarrative,
  performAction,
  pivotNarrative,
  randomLaunchConfig,
  resolveCrisis,
  rugPull,
  simulateOffline,
} from "../game/simulation";
import type { ActionId, MarketSpeed, WorldState } from "../game/types";

type WorldStore = {
  world: WorldState;
  selectedAction: ActionId;
  launchTicker: string;
  launchNarrative: string;
  setLaunchConfig: (ticker: string, narrative: string) => void;
  randomizeLaunch: () => void;
  launch: () => void;
  setSpeed: (speed: MarketSpeed) => void;
  selectAction: (actionId: ActionId) => void;
  performSelectedAction: () => void;
  tick: () => void;
  resolveCrisis: (crisisId: string, choiceId: string) => void;
  crisisFreeze: () => void;
  rugPull: () => void;
  pivot: () => void;
  catchUp: () => void;
};

const STORAGE_KEY = "pump-exe-world";

const loadWorld = (): WorldState => {
  try {
    const raw = localStorage.getItem(STORAGE_KEY);
    if (raw) {
      const saved = JSON.parse(raw) as WorldState;
      if (saved.tick !== undefined && saved.rngState !== undefined) return saved;
    }
  } catch { /* start fresh */ }
  return createWorld(Date.now());
};

const saveWorld = (world: WorldState) => {
  try { localStorage.setItem(STORAGE_KEY, JSON.stringify(world)); } catch { /* ok */ }
};

const firstAction = actionDefinitions[0].id;
const initialLaunch = randomLaunchConfig(Date.now());

export const useRunStore = create<WorldStore>((set, get) => ({
  world: loadWorld(),
  selectedAction: firstAction,
  launchTicker: initialLaunch.ticker,
  launchNarrative: initialLaunch.narrative,
  setLaunchConfig: (ticker, narrative) => set({ launchTicker: ticker, launchNarrative: narrative }),
  randomizeLaunch: () => {
    const cfg = randomLaunchConfig(Date.now());
    set({ launchTicker: cfg.ticker, launchNarrative: cfg.narrative });
  },
  launch: () => {
    const { world, launchTicker, launchNarrative } = get();
    const next = launchNarrative ? launchNarrative : "AI-powered engagement derivative";
    const updated = launchNarrative ? launchNarrative : next;
    const w = launchNarrative !== undefined ? launchNarrative : updated;
    const result = launchNarrative !== undefined
      ? performLaunch(world, launchTicker, launchNarrative)
      : performLaunch(world, launchTicker, "AI-powered engagement derivative");
    set({ world: result });
    saveWorld(result);
    const cfg = randomLaunchConfig(Date.now());
    set({ launchTicker: cfg.ticker, launchNarrative: cfg.narrative });
  },
  setSpeed: (speed) => {
    const { world } = get();
    const next = { ...world, speed };
    set({ world: next });
    saveWorld(next);
  },
  selectAction: (actionId) => set({ selectedAction: actionId }),
  performSelectedAction: () => {
    const { world, selectedAction } = get();
    const next = performAction(world, selectedAction);
    set({ world: next });
    saveWorld(next);
  },
  tick: () => {
    const { world } = get();
    const next = advanceTick(world);
    set({ world: next });
    if (next.tick % 5 === 0) saveWorld(next);
  },
  resolveCrisis: (crisisId, choiceId) => {
    const { world } = get();
    const next = resolveCrisis(world, crisisId, choiceId);
    set({ world: next });
    saveWorld(next);
  },
  crisisFreeze: () => {
    const { world } = get();
    const next = crisisFreeze(world);
    set({ world: next });
    saveWorld(next);
  },
  rugPull: () => {
    const { world } = get();
    const next = rugPull(world);
    set({ world: next });
    saveWorld(next);
  },
  pivot: () => {
    const { world } = get();
    const next = pivotNarrative(world);
    set({ world: next });
    saveWorld(next);
  },
  catchUp: () => {
    const { world } = get();
    const next = simulateOffline(world, 30);
    set({ world: next });
    saveWorld(next);
  },
}));

function performLaunch(world: WorldState, ticker: string, narrative: string): WorldState {
  return launchNarrative(world, ticker, narrative);
}
