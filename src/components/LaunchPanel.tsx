import { formatMoney } from "../game/rng";

type LaunchPanelProps = {
  ticker: string;
  narrative: string;
  cash: number;
  onConfig: (ticker: string, narrative: string) => void;
  onRandomize: () => void;
  onLaunch: () => void;
};

export function LaunchPanel({ ticker, narrative, cash, onConfig, onRandomize, onLaunch }: LaunchPanelProps) {
  return (
    <section className="panel p-6">
      <h2 className="section-title">launch new narrative</h2>
      <p className="mt-3 text-sm text-slate-400">
        The ecosystem is alive and moving. Deploy a new narrative to begin manipulating it.
      </p>

      <div className="mt-6 grid gap-4 sm:grid-cols-2">
        <div>
          <label className="mb-2 block font-mono text-xs uppercase tracking-[0.2em] text-slate-500">ticker</label>
          <input
            className="field"
            maxLength={6}
            value={ticker}
            onChange={(e) => onConfig(e.target.value.toUpperCase(), narrative)}
          />
        </div>
        <div>
          <label className="mb-2 block font-mono text-xs uppercase tracking-[0.2em] text-slate-500">narrative</label>
          <input
            className="field"
            value={narrative}
            onChange={(e) => onConfig(ticker, e.target.value)}
          />
        </div>
      </div>

      <div className="mt-6 flex flex-wrap items-center gap-3">
        <button className="button-primary" type="button" onClick={onLaunch}>
          launch narrative
        </button>
        <button className="button-secondary" type="button" onClick={onRandomize}>
          randomize
        </button>
        <span className="font-mono text-xs text-slate-500">balance: {formatMoney(cash)}</span>
      </div>
    </section>
  );
}
