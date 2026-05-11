type MetricCardProps = {
  label: string;
  value: string;
  tone?: "neutral" | "good" | "warn" | "danger";
  detail?: string;
};

const toneClass = {
  neutral: "border-cyan-400/20 text-cyan-100",
  good: "border-emerald-400/30 text-emerald-200",
  warn: "border-yellow-300/30 text-yellow-200",
  danger: "border-red-400/40 text-red-200",
};

export function MetricCard({ label, value, detail, tone = "neutral" }: MetricCardProps) {
  return (
    <div className={`panel p-4 ${toneClass[tone]}`}>
      <div className="text-xs uppercase tracking-[0.25em] text-slate-500">{label}</div>
      <div className="mt-2 font-mono text-2xl font-semibold">{value}</div>
      {detail ? <div className="mt-1 text-xs text-slate-500">{detail}</div> : null}
    </div>
  );
}
