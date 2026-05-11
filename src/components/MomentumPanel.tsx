import type { GameState } from "../game/types";

type MomentumPanelProps = {
  state: GameState;
};

export function MomentumPanel({ state }: MomentumPanelProps) {
  const chains = state.momentumChains;

  return (
    <section className="panel p-4">
      <div className="flex items-center justify-between">
        <h2 className="section-title">momentum chains</h2>
        <span className="font-mono text-xs text-slate-500">{chains.length}/5 active</span>
      </div>
      <div className="mt-4 space-y-3">
        {chains.length === 0 ? (
          <p className="rounded border border-slate-700/60 bg-black/20 p-3 text-sm text-slate-500">
            No combo window. The timeline is bored, which is expensive.
          </p>
        ) : (
          chains.map((chain) => {
            const remaining = Math.max(0, chain.expiresAt - state.tick);
            const width = Math.max(4, (remaining / Math.max(1, chain.expiresAt - chain.startedAt)) * 100);

            return (
              <article className="rounded border border-pink-400/20 bg-pink-950/10 p-3" key={chain.id}>
                <div className="flex items-center justify-between gap-3">
                  <div className="font-mono text-sm uppercase text-pink-100">{chain.label}</div>
                  <div className="font-mono text-xs text-pink-200">x{chain.intensity.toFixed(2)}</div>
                </div>
                <div className="mt-2 h-1.5 overflow-hidden rounded bg-slate-800">
                  <div className="h-full rounded bg-pink-300" style={{ width: `${width}%` }} />
                </div>
                <div className="mt-2 grid grid-cols-3 gap-2 text-xs text-slate-500">
                  <span>+hype {chain.hypeVelocity.toFixed(1)}</span>
                  <span>liq x{chain.liquidityMultiplier.toFixed(2)}</span>
                  <span>{remaining} ticks</span>
                </div>
              </article>
            );
          })
        )}
      </div>
    </section>
  );
}
