import type { MarketPoint } from "../game/types";

type MarketChartProps = {
  points: MarketPoint[];
  frenzyActive?: boolean;
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

export function MarketChart({ points, frenzyActive }: MarketChartProps) {
  const latest = points.at(-1);
  const prev = points.at(-2);
  const velocityChange = latest && prev ? latest.priceVelocity - prev.priceVelocity : 0;
  const isPumping = (latest?.priceVelocity ?? 0) > 0.15;
  const isDumping = (latest?.priceVelocity ?? 0) < -0.15;

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

  const strokeColor = frenzyActive
    ? "url(#frenzyGlow)"
    : isDumping
      ? "#f87171"
      : isPumping
        ? "#22d3ee"
        : "url(#priceGlow)";

  const strokeWidth = frenzyActive ? 4 : Math.abs(latest?.priceVelocity ?? 0) > 0.3 ? 4 : 3;

  return (
    <section className={`panel col-span-2 p-4 transition-all ${frenzyActive ? "border-pink-400/40 shadow-[0_0_40px_rgba(236,72,153,0.12)]" : ""}`}>
      <div className="mb-3 flex items-center justify-between">
        <h2 className="section-title">belief candle</h2>
        <div className="flex items-center gap-3">
          {isPumping && <span className="animate-pulse font-mono text-xs text-emerald-300">PUMPING</span>}
          {isDumping && <span className="animate-pulse font-mono text-xs text-red-300">DUMPING</span>}
          {frenzyActive && <span className="animate-pulse font-mono text-xs text-pink-300">FRENZY</span>}
          <span className={`font-mono text-xs ${(latest?.priceVelocity ?? 0) < 0 ? "text-red-300" : "text-emerald-300"}`}>
            {latest ? `${latest.priceVelocity >= 0 ? "+" : ""}${Math.round(latest.priceVelocity * 100)}% velocity` : `${points.length} ticks sampled`}
          </span>
        </div>
      </div>
      <svg
        className={`h-48 w-full overflow-visible rounded border bg-black/30 p-4 transition-all ${
          frenzyActive ? "border-pink-400/30" : isDumping ? "border-red-400/20" : isPumping ? "border-emerald-400/20" : "border-cyan-400/10"
        }`}
        viewBox="0 0 560 160"
      >
        <defs>
          <linearGradient id="priceGlow" x1="0%" x2="100%" y1="0%" y2="0%">
            <stop offset="0%" stopColor="#22d3ee" />
            <stop offset="100%" stopColor="#f472b6" />
          </linearGradient>
          <linearGradient id="frenzyGlow" x1="0%" x2="100%" y1="0%" y2="0%">
            <stop offset="0%" stopColor="#ec4899" />
            <stop offset="50%" stopColor="#a855f7" />
            <stop offset="100%" stopColor="#ec4899" />
          </linearGradient>
          <filter id="glow">
            <feGaussianBlur stdDeviation="3" result="blur" />
            <feMerge>
              <feMergeNode in="blur" />
              <feMergeNode in="SourceGraphic" />
            </feMerge>
          </filter>
        </defs>

        {points.map((point, index) => {
          const width = 560 / Math.max(1, points.length);
          const height = Math.max(2, (point.volume / maxVolume) * 48);
          const x = index * width;
          const y = 160 - height;
          const barColor = point.event?.type === "whale"
            ? "#60a5fa"
            : point.event?.type === "panic"
              ? "#ef4444"
              : point.event?.type === "euphoria"
                ? "#10b981"
                : point.event?.type === "frenzy"
                  ? "#ec4899"
                  : point.priceVelocity < 0 ? "#f87171" : "#22d3ee";
          const barOpacity = point.event ? 0.4 : 0.18;
          return <rect fill={barColor} fillOpacity={barOpacity} height={height} key={`${point.tick}-vol`} width={Math.max(1, width - 1)} x={x} y={y} />;
        })}

        <path d={stabilityPath} fill="none" stroke="#a78bfa" strokeDasharray="2 7" strokeOpacity="0.5" strokeWidth="2" />
        <path d={hypePath} fill="none" stroke="#facc15" strokeDasharray="5 6" strokeOpacity="0.45" strokeWidth="2" />
        <path
          d={pricePath}
          fill="none"
          stroke={strokeColor}
          strokeLinecap="round"
          strokeLinejoin="round"
          strokeWidth={strokeWidth}
          filter={frenzyActive || Math.abs(velocityChange) > 0.2 ? "url(#glow)" : undefined}
        />

        {points.map((point, index) => {
          if (!point.event) return null;
          const x = (index / Math.max(1, points.length - 1)) * 560;
          const markerColor =
            point.event.type === "whale" ? "#60a5fa"
            : point.event.type === "panic" ? "#ef4444"
            : point.event.type === "euphoria" ? "#10b981"
            : point.event.type === "crash" ? "#ff0000"
            : "#ec4899";
          return (
            <g key={`event-${point.tick}`}>
              <circle cx={x} cy={8} r={4 + point.event.intensity * 3} fill={markerColor} fillOpacity={0.6} />
              <circle cx={x} cy={8} r={2} fill={markerColor} />
            </g>
          );
        })}
      </svg>
      <div className="mt-3 flex flex-wrap gap-4 text-xs uppercase tracking-[0.2em] text-slate-500">
        <span className="text-cyan-300">price</span>
        <span className="text-yellow-200/80">hype</span>
        <span className="text-violet-300/80">stability</span>
        <span className="text-slate-400">volume</span>
        <span className="text-blue-300/80">whale</span>
        <span className="text-red-300/80">panic</span>
        <span className="text-emerald-300/80">euphoria</span>
      </div>
    </section>
  );
}
