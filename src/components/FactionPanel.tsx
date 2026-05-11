import type { FactionState } from "../game/factions";

type FactionPanelProps = {
  factions: FactionState[];
};

const factionColor: Record<string, string> = {
  retailSwarm: "border-emerald-400/20 text-emerald-200",
  aiCultists: "border-violet-400/20 text-violet-200",
  regulators: "border-yellow-300/20 text-yellow-200",
  whaleSyndicate: "border-blue-400/20 text-blue-200",
  doomers: "border-red-400/20 text-red-200",
  conspiracyAccounts: "border-purple-400/20 text-purple-200",
  trendChasers: "border-orange-300/20 text-orange-200",
};

export function FactionPanel({ factions }: FactionPanelProps) {
  const activeFactions = factions.filter((f) => f.activity > 30).sort((a, b) => b.activity - a.activity);

  return (
    <section className="panel p-4">
      <div className="flex items-center justify-between">
        <h2 className="section-title">social factions</h2>
        <span className="font-mono text-xs text-slate-500">{activeFactions.length} active</span>
      </div>
      <div className="mt-4 grid grid-cols-2 gap-2 lg:grid-cols-3 xl:grid-cols-4">
        {factions.map((faction) => {
          const colorClass = factionColor[faction.id] ?? "border-slate-400/20 text-slate-200";
          const isActive = faction.activity > 40;
          return (
            <div
              className={`rounded border bg-black/20 p-2.5 transition-all ${colorClass} ${isActive ? "opacity-100" : "opacity-50"}`}
              key={faction.id}
            >
              <div className="font-mono text-xs uppercase">{faction.label}</div>
              <div className="mt-1.5 flex items-end justify-between gap-1">
                <div className="text-[10px] text-slate-500">pwr {Math.round(faction.power)}</div>
                <div className="text-[10px] text-slate-500">act {Math.round(faction.activity)}</div>
              </div>
              <div className="mt-1.5 h-1 overflow-hidden rounded bg-slate-800">
                <div
                  className="h-full rounded transition-all"
                  style={{
                    width: `${Math.max(3, faction.activity)}%`,
                    background: faction.activity > 60 ? "rgb(236, 72, 153)" : "rgb(148, 163, 184)",
                  }}
                />
              </div>
            </div>
          );
        })}
      </div>
    </section>
  );
}
