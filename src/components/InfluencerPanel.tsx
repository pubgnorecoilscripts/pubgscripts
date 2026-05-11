import type { Influencer } from "../game/types";

type InfluencerPanelProps = {
  influencers: Influencer[];
};

export function InfluencerPanel({ influencers }: InfluencerPanelProps) {
  return (
    <section className="panel p-4">
      <h2 className="section-title">influencer liabilities</h2>
      <div className="mt-4 space-y-3">
        {influencers.map((influencer) => (
          <article className="rounded border border-cyan-400/10 bg-black/20 p-3" key={influencer.id}>
            <div className="flex items-start justify-between gap-3">
              <div>
                <div className="font-mono text-sm text-cyan-100">{influencer.handle}</div>
                <div className="mt-1 text-xs text-slate-500">{influencer.archetype}</div>
              </div>
              <span className={`rounded px-2 py-1 font-mono text-xs ${influencer.status === "rogue" ? "bg-red-400/10 text-red-200" : "bg-emerald-400/10 text-emerald-200"}`}>
                {influencer.status}
              </span>
            </div>
            <div className="mt-3 grid grid-cols-3 gap-2 text-xs text-slate-500">
              <Meter label="reach" value={influencer.reach} />
              <Meter label="cred" value={influencer.credibility} />
              <Meter label="loyal" value={influencer.alignment} />
            </div>
          </article>
        ))}
      </div>
    </section>
  );
}

function Meter({ label, value }: { label: string; value: number }) {
  return (
    <div>
      <div className="mb-1 flex justify-between">
        <span>{label}</span>
        <span className="font-mono text-slate-300">{Math.round(value)}</span>
      </div>
      <div className="h-1.5 overflow-hidden rounded bg-slate-800">
        <div className="h-full rounded bg-cyan-300" style={{ width: `${Math.max(2, Math.min(100, value))}%` }} />
      </div>
    </div>
  );
}
