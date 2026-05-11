import type { MarketPoint } from "../game/types";

type MarketChartProps = {
  points: MarketPoint[];
};

const buildPath = (values: number[], width: number, height: number) => {
  if (values.length < 2) {
    return "";
  }

  const min = Math.min(...values);
  const max = Math.max(...values);
  const range = max - min || 1;

  return values
    .map((value, index) => {
      const x = (index / (values.length - 1)) * width;
      const y = height - ((value - min) / range) * height;
      return `${index === 0 ? "M" : "L"} ${x.toFixed(2)} ${y.toFixed(2)}`;
    })
    .join(" ");
};

export function MarketChart({ points }: MarketChartProps) {
  const latest = points.at(-1);
  const pricePath = buildPath(
    points.map((point) => point.price),
    560,
    160,
  );
  const hypePath = buildPath(
    points.map((point) => point.hype),
    560,
    160,
  );
  const stabilityPath = buildPath(
    points.map((point) => point.narrativeStability),
    560,
    160,
  );
  const maxVolume = Math.max(1, ...points.map((point) => point.volume));

  return (
    <section className="panel col-span-2 p-4">
      <div className="mb-3 flex items-center justify-between">
        <h2 className="section-title">belief candle</h2>
        <span className={`font-mono text-xs ${(latest?.priceVelocity ?? 0) < 0 ? "text-red-300" : "text-emerald-300"}`}>
          {latest ? `${latest.priceVelocity >= 0 ? "+" : ""}${Math.round(latest.priceVelocity * 100)}% velocity` : `${points.length} ticks sampled`}
        </span>
      </div>
      <svg className="h-48 w-full overflow-visible rounded border border-cyan-400/10 bg-black/30 p-4" viewBox="0 0 560 160">
        <defs>
          <linearGradient id="priceGlow" x1="0%" x2="100%" y1="0%" y2="0%">
            <stop offset="0%" stopColor="#22d3ee" />
            <stop offset="100%" stopColor="#f472b6" />
          </linearGradient>
        </defs>
        {points.map((point, index) => {
          const width = 560 / Math.max(1, points.length);
          const height = Math.max(2, (point.volume / maxVolume) * 48);
          const x = index * width;
          const y = 160 - height;
          return <rect fill={point.priceVelocity < 0 ? "#f87171" : "#22d3ee"} fillOpacity="0.18" height={height} key={`${point.tick}-vol`} width={Math.max(1, width - 1)} x={x} y={y} />;
        })}
        <path d={stabilityPath} fill="none" stroke="#a78bfa" strokeDasharray="2 7" strokeOpacity="0.5" strokeWidth="2" />
        <path d={hypePath} fill="none" stroke="#facc15" strokeDasharray="5 6" strokeOpacity="0.45" strokeWidth="2" />
        <path d={pricePath} fill="none" stroke="url(#priceGlow)" strokeLinecap="round" strokeLinejoin="round" strokeWidth="3" />
      </svg>
      <div className="mt-3 flex gap-5 text-xs uppercase tracking-[0.2em] text-slate-500">
        <span className="text-cyan-300">price</span>
        <span className="text-yellow-200/80">hype</span>
        <span className="text-violet-300/80">stability</span>
        <span className="text-slate-400">volume bars</span>
      </div>
    </section>
  );
}
