import type { Narrative } from "../game/types";

type MomentumPanelProps = {
  narr: Narrative;
  tick: number;
};

export function MomentumPanel({ narr, tick }: MomentumPanelProps) {
  const chains = narr.momentumChains;
  const { frenzy } = narr;

  return (
    <section className={`panel p-4 transition-all ${frenzy.active ? "border-pink-400/30 shadow-[0_0_20px_rgba(236,72,153,0.08)]" : ""}`}>
      <div className="flex items-center justify-between">
        <h2 className="section-title">momentum chains</h2>
        <div className="flex items-center gap-3">
          {frenzy.active && <span className="animate-pulse rounded bg-pink-500/20 px-2 py-0.5 font-mono text-[10px] uppercase text-pink-200">FRENZY</span>}
          <span className="font-mono text-xs text-slate-500">{chains.length}/6 active</span>
        </div>
      </div>
      <div className="mt-4 space-y-3">
        {chains.length === 0 && !frenzy.active ? (
          <p className="rounded border border-slate-700/60 bg-black/20 p-3 text-sm text-slate-500">
            No combo window. The timeline is bored, which is expensive.
          </p>
        ) : (
          <>
            {frenzy.active && (
              <article className="rounded border border-pink-400/30 bg-pink-950/20 p-3 animate-pulse">
                <div className="flex items-center justify-between gap-3">
                  <div className="font-mono text-sm uppercase text-pink-100">NARRATIVE FRENZY</div>
                  <div className="font-mono text-xs text-pink-200">x{frenzy.intensity.toFixed(2)}</div>
                </div>
                <div className="mt-2 h-1.5 overflow-hidden rounded bg-slate-800">
                  <div className="h-full rounded bg-gradient-to-r from-pink-400 to-purple-400" style={{ width: `${(frenzy.ticksRemaining / 20) * 100}%` }} />
                </div>
                <div className="mt-2 grid grid-cols-3 gap-2 text-xs text-pink-300/80">
                  <span>hype x{(1 + frenzy.intensity * 0.6).toFixed(2)}</span>
                  <span>crisis +{Math.round(frenzy.intensity * 40)}%</span>
                  <span>{frenzy.ticksRemaining} ticks</span>
                </div>
              </article>
            )}
            {chains.map((chain) => {
              const remaining = Math.max(0, chain.expiresAt - tick);
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
            })}
          </>
        )}
      </div>
    </section>
  );
}
