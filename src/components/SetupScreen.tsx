import type { CoinConfig } from "../game/types";

type SetupScreenProps = {
  config: CoinConfig;
  onConfig: (config: Partial<CoinConfig>) => void;
  onRandomize: () => void;
  onStart: () => void;
};

export function SetupScreen({ config, onConfig, onRandomize, onStart }: SetupScreenProps) {
  return (
    <main className="min-h-screen bg-[radial-gradient(circle_at_top,#122238_0,#05070a_52%)] p-8 text-slate-100">
      <section className="mx-auto grid max-w-6xl gap-6 lg:grid-cols-[1.1fr_0.9fr]">
        <div className="panel p-8">
          <div className="text-xs uppercase tracking-[0.35em] text-cyan-300">near future capitalism exploit kit</div>
          <h1 className="mt-4 font-mono text-6xl font-black uppercase tracking-tight text-white">Pump.exe</h1>
          <p className="mt-5 max-w-2xl text-lg leading-8 text-slate-300">
            Manufacture belief faster than reality collapses. This prototype focuses on the addictive core loop:
            hype, liquidity, scandals, panic, and the exit button you should probably not press yet.
          </p>
          <div className="mt-8 grid gap-4 sm:grid-cols-3">
            <div className="rounded border border-cyan-300/20 bg-cyan-300/5 p-4">
              <div className="font-mono text-2xl text-cyan-200">1</div>
              <div className="mt-2 text-sm text-slate-400">Create a cursed narrative.</div>
            </div>
            <div className="rounded border border-pink-300/20 bg-pink-300/5 p-4">
              <div className="font-mono text-2xl text-pink-200">2</div>
              <div className="mt-2 text-sm text-slate-400">Convert attention into unstable liquidity.</div>
            </div>
            <div className="rounded border border-red-300/20 bg-red-300/5 p-4">
              <div className="font-mono text-2xl text-red-200">3</div>
              <div className="mt-2 text-sm text-slate-400">Survive the story you created.</div>
            </div>
          </div>
        </div>

        <form
          className="panel p-6"
          onSubmit={(event) => {
            event.preventDefault();
            onStart();
          }}
        >
          <h2 className="section-title">new pump</h2>
          <label className="mt-6 block text-sm text-slate-400">
            Ticker
            <input
              className="field mt-2"
              maxLength={6}
              value={config.ticker}
              onChange={(event) => onConfig({ ticker: event.target.value })}
            />
          </label>
          <label className="mt-4 block text-sm text-slate-400">
            Narrative
            <textarea
              className="field mt-2 min-h-28 resize-none"
              value={config.narrative}
              onChange={(event) => onConfig({ narrative: event.target.value })}
            />
          </label>
          <label className="mt-4 block text-sm text-slate-400">
            Seed
            <input
              className="field mt-2"
              type="number"
              value={config.seed}
              onChange={(event) => onConfig({ seed: Number(event.target.value) })}
            />
          </label>
          <div className="mt-6 grid gap-3 sm:grid-cols-2">
            <button className="button-secondary" type="button" onClick={onRandomize}>
              randomize grift
            </button>
            <button className="button-primary" type="submit">
              launch coin
            </button>
          </div>
        </form>
      </section>
    </main>
  );
}
