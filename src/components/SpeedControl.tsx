import type { MarketSpeed } from "../game/types";

type SpeedControlProps = {
  speed: MarketSpeed;
  onSpeed: (speed: MarketSpeed) => void;
};

const speeds: { id: MarketSpeed; label: string; multiplier: string }[] = [
  { id: "focus", label: "FOCUS", multiplier: "0.5x" },
  { id: "normal", label: "NORMAL", multiplier: "1.0x" },
  { id: "frenzy", label: "FRENZY", multiplier: "1.5x" },
  { id: "meltdown", label: "MELTDOWN", multiplier: "2.0x" },
];

const speedColors: Record<MarketSpeed, string> = {
  focus: "border-blue-400/50 bg-blue-950/40 text-blue-200",
  normal: "border-cyan-300/50 bg-cyan-950/30 text-cyan-200",
  frenzy: "border-pink-400/50 bg-pink-950/40 text-pink-200",
  meltdown: "border-red-400/60 bg-red-950/50 text-red-200",
};

export function SpeedControl({ speed, onSpeed }: SpeedControlProps) {
  return (
    <div className="flex items-center gap-1 rounded-lg border border-slate-700/40 bg-black/30 p-1">
      {speeds.map((s) => {
        const active = speed === s.id;
        return (
          <button
            key={s.id}
            type="button"
            onClick={() => onSpeed(s.id)}
            className={`rounded-md px-2.5 py-1.5 font-mono text-[10px] font-bold uppercase tracking-[0.15em] transition-all ${
              active ? speedColors[s.id] : "text-slate-600 hover:text-slate-400"
            }`}
          >
            {s.multiplier}
          </button>
        );
      })}
    </div>
  );
}
