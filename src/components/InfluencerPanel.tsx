import type { Influencer } from "../game/types";

type InfluencerPanelProps = {
  influencers: Influencer[];
};

const statusColors: Record<string, string> = {
  loyal: "bg-emerald-400/10 text-emerald-200",
  bored: "bg-yellow-400/10 text-yellow-200",
  rogue: "bg-red-400/10 text-red-200",
  cancelled: "bg-slate-700/20 text-slate-500",
  ascended: "bg-purple-400/10 text-purple-200",
};

export function InfluencerPanel({ influencers }: InfluencerPanelProps) {
  return (
    <section className="panel p-4">
      <h2 className="section-title">influencer liabilities</h2>
      <div className="mt-4 space-y-3">
        {influencers.map((influencer) => {
          const dangerLevel = influencer.autonomy > 60 ? "border-red-400/20" : influencer.autonomy > 40 ? "border-yellow-400/10" : "border-cyan-400/10";
          return (
            <article className={`rounded border bg-black/20 p-3 ${dangerLevel}`} key={influencer.id}>
              <div className="flex items-start justify-between gap-3">
                <div>
                  <div className="font-mono text-sm text-cyan-100">{influencer.handle}</div>
                  <div className="mt-1 flex items-center gap-2 text-xs text-slate-500">
                    <span>{influencer.archetype}</span>
                    <span className="rounded bg-slate-800 px-1.5 py-0.5 font-mono text-[10px] text-slate-400">{influencer.trait}</span>
                  </div>
                </div>
                <span className={`rounded px-2 py-1 font-mono text-xs ${statusColors[influencer.status] ?? statusColors.loyal}`}>
                  {influencer.status}
                </span>
              </div>
              <div className="mt-3 grid grid-cols-4 gap-2 text-xs text-slate-500">
                <Meter label="reach" value={influencer.reach} />
                <Meter label="loyal" value={influencer.alignment} color={influencer.alignment < 30 ? "#f87171" : "#22d3ee"} />
                <Meter label="auto" value={influencer.autonomy} color={influencer.autonomy > 60 ? "#f472b6" : "#94a3b8"} />
                <Meter label="audience" value={Math.min(100, influencer.audience / 1000)} />
              </div>
            </article>
          );
        })}
      </div>
    </section>
  );
}

function Meter({ label, value, color }: { label: string; value: number; color?: string }) {
  return (
    <div>
      <div className="mb-1 flex justify-between">
        <span>{label}</span>
        <span className="font-mono text-slate-300">{Math.round(value)}</span>
      </div>
      <div className="h-1.5 overflow-hidden rounded bg-slate-800">
        <div className="h-full rounded" style={{ width: `${Math.max(2, Math.min(100, value))}%`, background: color ?? "#22d3ee" }} />
      </div>
    </div>
  );
}
