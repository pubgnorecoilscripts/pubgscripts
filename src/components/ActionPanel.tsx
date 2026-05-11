import { actionDefinitions } from "../game/content";
import { formatMoney } from "../game/rng";
import type { ActionId, WorldState } from "../game/types";

type ActionPanelProps = {
  world: WorldState;
  selectedAction: ActionId;
  onSelect: (actionId: ActionId) => void;
  onExecute: () => void;
  onCrisisFreeze: () => void;
  onPivot: () => void;
  onRug: () => void;
};

export function ActionPanel({ world, selectedAction, onSelect, onExecute, onCrisisFreeze, onPivot, onRug }: ActionPanelProps) {
  const narr = world.activeNarrative;
  if (!narr) return null;

  const selected = actionDefinitions.find((a) => a.id === selectedAction) ?? actionDefinitions[0];
  const canAct = narr.state === "active" && world.cash >= selected.cost && (narr.cooldowns[selected.id] ?? 0) === 0;
  const activeSynergy = narr.momentumChains.reduce((m, c) => m * (c.synergy[selectedAction] ?? 1), 1);
  const canFreeze = narr.state === "active" && narr.crisisFreezeCooldown === 0 && world.cash >= 800 && narr.crises.length > 0;

  return (
    <section className="panel p-4">
      <div className="flex items-center justify-between">
        <h2 className="section-title">operator console</h2>
        <span className="font-mono text-xs text-slate-500">T+{world.tick}</span>
      </div>

      <div className="mt-4 grid gap-3">
        {actionDefinitions.map((action) => {
          const active = selectedAction === action.id;
          const cooldown = narr.cooldowns[action.id] ?? 0;
          const isDangerous = action.dangerous;
          return (
            <button
              className={`action-card text-left ${active ? "action-card-active" : ""} ${isDangerous && active ? "action-card-dangerous" : ""}`}
              key={action.id}
              onClick={() => onSelect(action.id)}
              type="button"
            >
              <div className="flex items-start justify-between gap-3">
                <div>
                  <div className={`font-mono text-sm uppercase ${isDangerous ? "text-red-100" : "text-slate-100"}`}>
                    {isDangerous && <span className="mr-1.5 text-red-400">⚠</span>}
                    {action.label}
                  </div>
                  <div className="mt-1 text-xs text-slate-500">{action.command}</div>
                </div>
                <div className="text-right font-mono text-xs text-cyan-200">{formatMoney(action.cost)}</div>
              </div>
              <p className="mt-2 text-xs leading-5 text-slate-400">{action.description}</p>
              <div className="mt-2 flex flex-wrap gap-2 font-mono text-[10px] uppercase tracking-[0.16em] text-slate-500">
                <span>{action.chain.duration} tick window</span>
                <span>liq x{action.chain.liquidityMultiplier.toFixed(2)}</span>
                {isDangerous && action.delayedRisk && <span className="text-red-300">{Math.round(action.delayedRisk.chance * 100)}% blowback</span>}
              </div>
              {cooldown > 0 ? <div className="mt-2 text-xs uppercase tracking-[0.2em] text-yellow-300">{cooldown} tick cooldown</div> : null}
            </button>
          );
        })}
      </div>

      {activeSynergy > 1.01 && (
        <div className="mt-4 rounded border border-pink-400/30 bg-pink-950/20 p-3 font-mono text-xs uppercase tracking-[0.18em] text-pink-100">
          combo: x{activeSynergy.toFixed(2)}
        </div>
      )}

      {narr.frenzy.active ? (
        <div className="mt-3 rounded border border-pink-400/40 bg-pink-950/30 p-3 font-mono text-xs uppercase tracking-[0.18em] text-pink-200 animate-pulse">
          FRENZY x{narr.frenzy.intensity.toFixed(1)} — {narr.frenzy.ticksRemaining} ticks
        </div>
      ) : narr.frenzy.actionsInWindow > 0 ? (
        <div className="mt-3 rounded border border-slate-600/40 bg-slate-900/50 p-3 font-mono text-xs uppercase tracking-[0.18em] text-slate-400">
          combo: {narr.frenzy.actionsInWindow}/3
        </div>
      ) : null}

      {narr.delayedRisks.length > 0 && (
        <div className="mt-3 rounded border border-red-400/20 bg-red-950/10 p-3 font-mono text-[10px] uppercase tracking-[0.16em] text-red-300">
          {narr.delayedRisks.length} delayed consequence{narr.delayedRisks.length > 1 ? "s" : ""} ticking
        </div>
      )}

      <div className="mt-5 grid grid-cols-2 gap-3">
        <button className="button-primary col-span-2" disabled={!canAct} type="button" onClick={onExecute}>execute selected</button>
        <button className="button-secondary" disabled={!canFreeze} type="button" onClick={onCrisisFreeze}>
          {narr.crisisFreezeCooldown > 0 ? `freeze (${narr.crisisFreezeCooldown})` : "crisis freeze"}
        </button>
        <button className="button-secondary" disabled={narr.state !== "active" || world.cash < 1500} type="button" onClick={onPivot}>pivot narrative</button>
        <button className="button-danger col-span-2" disabled={narr.state !== "active"} type="button" onClick={onRug}>rug pull & extract</button>
      </div>
    </section>
  );
}
