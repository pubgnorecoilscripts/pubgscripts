import { ActionPanel } from "./components/ActionPanel";
import { CrisisPanel } from "./components/CrisisPanel";
import { FeedPanel } from "./components/FeedPanel";
import { InfluencerPanel } from "./components/InfluencerPanel";
import { MarketChart } from "./components/MarketChart";
import { MetricCard } from "./components/MetricCard";
import { SetupScreen } from "./components/SetupScreen";
import { TerminalPanel } from "./components/TerminalPanel";
import { formatMoney } from "./game/rng";
import { useRunStore } from "./store/runStore";

const percent = (value: number) => `${Math.round(value)}%`;

export default function App() {
  const {
    config,
    state,
    selectedAction,
    setConfig,
    randomizeConfig,
    start,
    reset,
    selectAction,
    performSelectedAction,
    tick,
    resolveCrisis,
    rugPull,
    pivot,
  } = useRunStore();

  if (!state.coin) {
    return <SetupScreen config={config} onConfig={setConfig} onRandomize={randomizeConfig} onStart={start} />;
  }

  const { coin } = state;
  const stressLevel = Math.max(coin.heat, 100 - coin.trust, coin.volatility);

  return (
    <main className={`min-h-screen bg-[#05070a] p-4 text-slate-100 md:p-6 ${stressLevel > 72 ? "stress-mode" : ""}`}>
      <header className="mb-5 flex flex-col gap-4 border-b border-cyan-300/10 pb-4 lg:flex-row lg:items-end lg:justify-between">
        <div>
          <div className="text-xs uppercase tracking-[0.35em] text-cyan-300">Pump.exe active run</div>
          <h1 className="mt-2 font-mono text-4xl font-black uppercase tracking-tight text-white">
            ${coin.ticker} <span className="text-slate-600">/</span> {coin.narrative}
          </h1>
        </div>
        <div className="flex flex-wrap gap-2">
          <StatusPill phase={state.phase} />
          <button
            className="button-secondary"
            type="button"
            onClick={() => void window.pumpDesktop?.saveRun(`run-${state.seed}`, state)}
            disabled={!window.pumpDesktop}
          >
            sqlite save
          </button>
        </div>
      </header>

      <section className="mb-5 grid gap-3 sm:grid-cols-2 lg:grid-cols-6">
        <MetricCard label="cash" value={formatMoney(state.cash)} detail="operator balance" tone="good" />
        <MetricCard label="price" value={formatMoney(coin.price)} detail="belief per token" />
        <MetricCard label="liquidity" value={formatMoney(coin.liquidity)} detail="available exit meat" />
        <MetricCard label="hype" value={percent(coin.hype)} detail="narrative oxygen" tone={coin.hype > 70 ? "warn" : "neutral"} />
        <MetricCard label="trust" value={percent(coin.trust)} detail="collapse fuse" tone={coin.trust < 30 ? "danger" : "good"} />
        <MetricCard label="heat" value={percent(coin.heat)} detail="adult supervision" tone={coin.heat > 70 ? "danger" : "neutral"} />
      </section>

      {state.phase !== "running" ? (
        <section className="panel mb-5 border-red-400/40 bg-red-950/20 p-5">
          <h2 className="font-mono text-2xl uppercase text-red-100">
            {state.phase === "cashedOut" ? "you escaped with the bag" : "run collapsed"}
          </h2>
          <p className="mt-2 text-slate-300">
            Peak market cap: {formatMoney(state.stats.peakMarketCap)}. Crises survived: {state.stats.crisesSurvived}. Actions taken:{" "}
            {state.stats.actionsTaken}.
          </p>
        </section>
      ) : null}

      <div className="grid gap-5 xl:grid-cols-[360px_minmax(0,1fr)_360px]">
        <div className="space-y-5">
          <ActionPanel
            state={state}
            selectedAction={selectedAction}
            onSelect={selectAction}
            onExecute={performSelectedAction}
            onTick={tick}
            onPivot={pivot}
            onRug={rugPull}
            onReset={reset}
          />
          <InfluencerPanel influencers={state.influencers} />
        </div>

        <div className="space-y-5">
          <MarketChart points={state.market} />
          <CrisisPanel crises={state.crises} onResolve={resolveCrisis} />
          <TerminalPanel lines={state.terminal} />
        </div>

        <FeedPanel feed={state.feed} />
      </div>
    </main>
  );
}

function StatusPill({ phase }: { phase: string }) {
  const label = phase === "running" ? "belief engine live" : phase === "cashedOut" ? "bag secured" : "postmortem";
  return <span className="rounded border border-cyan-300/20 bg-cyan-300/5 px-3 py-2 font-mono text-xs uppercase tracking-[0.2em] text-cyan-100">{label}</span>;
}
