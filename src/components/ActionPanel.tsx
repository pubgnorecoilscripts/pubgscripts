import { actionDefinitions } from "../game/content";
import { formatMoney } from "../game/rng";
import type { ActionId, GameState } from "../game/types";

type ActionPanelProps = {
  state: GameState;
  selectedAction: ActionId;
  onSelect: (actionId: ActionId) => void;
  onExecute: () => void;
  onTick: () => void;
  onPivot: () => void;
  onRug: () => void;
  onReset: () => void;
};

export function ActionPanel({
  state,
  selectedAction,
  onSelect,
  onExecute,
  onTick,
  onPivot,
  onRug,
  onReset,
}: ActionPanelProps) {
  const selected = actionDefinitions.find((action) => action.id === selectedAction) ?? actionDefinitions[0];
  const canAct = state.phase === "running" && state.cash >= selected.cost && (state.cooldowns[selected.id] ?? 0) === 0;

  return (
    <section className="panel p-4">
      <div className="flex items-center justify-between">
        <h2 className="section-title">operator console</h2>
        <span className="font-mono text-xs text-slate-500">T+{state.tick}</span>
      </div>

      <div className="mt-4 grid gap-3">
        {actionDefinitions.map((action) => {
          const active = selectedAction === action.id;
          const cooldown = state.cooldowns[action.id] ?? 0;
          return (
            <button
              className={`action-card text-left ${active ? "action-card-active" : ""}`}
              key={action.id}
              onClick={() => onSelect(action.id)}
              type="button"
            >
              <div className="flex items-start justify-between gap-3">
                <div>
                  <div className="font-mono text-sm uppercase text-slate-100">{action.label}</div>
                  <div className="mt-1 text-xs text-slate-500">{action.command}</div>
                </div>
                <div className="text-right font-mono text-xs text-cyan-200">{formatMoney(action.cost)}</div>
              </div>
              <p className="mt-2 text-xs leading-5 text-slate-400">{action.description}</p>
              {cooldown > 0 ? <div className="mt-2 text-xs uppercase tracking-[0.2em] text-yellow-300">{cooldown} tick cooldown</div> : null}
            </button>
          );
        })}
      </div>

      <div className="mt-5 grid grid-cols-2 gap-3">
        <button className="button-primary col-span-2" disabled={!canAct} type="button" onClick={onExecute}>
          execute selected
        </button>
        <button className="button-secondary" disabled={state.phase !== "running"} type="button" onClick={onTick}>
          advance feed
        </button>
        <button className="button-secondary" disabled={state.phase !== "running" || state.cash < 1500} type="button" onClick={onPivot}>
          pivot narrative
        </button>
        <button className="button-danger" disabled={state.phase !== "running"} type="button" onClick={onRug}>
          rug pull
        </button>
        <button className="button-secondary" type="button" onClick={onReset}>
          new run
        </button>
      </div>
    </section>
  );
}
