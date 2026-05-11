import { create } from "zustand";
import { actionDefinitions } from "../game/content";
import {
  advanceTick,
  createSetupState,
  performAction,
  pivotNarrative,
  randomCoinConfig,
  resolveCrisis,
  rugPull,
  startRun,
} from "../game/simulation";
import type { ActionId, CoinConfig, GameState } from "../game/types";

type RunStore = {
  config: CoinConfig;
  state: GameState;
  selectedAction: ActionId;
  setConfig: (config: Partial<CoinConfig>) => void;
  randomizeConfig: () => void;
  start: () => void;
  reset: () => void;
  selectAction: (actionId: ActionId) => void;
  performSelectedAction: () => void;
  tick: () => void;
  resolveCrisis: (crisisId: string, choiceId: string) => void;
  rugPull: () => void;
  pivot: () => void;
};

const initialConfig = randomCoinConfig(13_337);
const firstAction = actionDefinitions[0].id;

export const useRunStore = create<RunStore>((set, get) => ({
  config: initialConfig,
  state: createSetupState(initialConfig.seed),
  selectedAction: firstAction,
  setConfig: (config) =>
    set((store) => ({
      config: {
        ...store.config,
        ...config,
        ticker: config.ticker?.toUpperCase().slice(0, 6) ?? store.config.ticker,
      },
    })),
  randomizeConfig: () => {
    const config = randomCoinConfig(Date.now());
    set({ config, state: createSetupState(config.seed) });
  },
  start: () => {
    const { config } = get();
    set({ state: startRun(config) });
  },
  reset: () => {
    const config = randomCoinConfig(Date.now());
    set({ config, state: createSetupState(config.seed), selectedAction: firstAction });
  },
  selectAction: (actionId) => set({ selectedAction: actionId }),
  performSelectedAction: () => {
    const { state, selectedAction } = get();
    set({ state: performAction(state, selectedAction) });
  },
  tick: () => {
    const { state } = get();
    set({ state: advanceTick(state).state });
  },
  resolveCrisis: (crisisId, choiceId) => {
    const { state } = get();
    set({ state: resolveCrisis(state, crisisId, choiceId) });
  },
  rugPull: () => {
    const { state } = get();
    set({ state: rugPull(state) });
  },
  pivot: () => {
    const { state } = get();
    set({ state: pivotNarrative(state) });
  },
}));
