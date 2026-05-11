import { useEffect } from "react";
import { ActionPanel } from "./components/ActionPanel";
import { CrisisPanel } from "./components/CrisisPanel";
import { FactionPanel } from "./components/FactionPanel";
import { FeedPanel } from "./components/FeedPanel";
import { FrenzyOverlay } from "./components/FrenzyOverlay";
import { InfluencerPanel } from "./components/InfluencerPanel";
import { LaunchPanel } from "./components/LaunchPanel";
import { MarketChart } from "./components/MarketChart";
import { MetricCard } from "./components/MetricCard";
import { MomentumPanel } from "./components/MomentumPanel";
import { SpeedControl } from "./components/SpeedControl";
import { TerminalPanel } from "./components/TerminalPanel";
import { formatMoney } from "./game/rng";
import { useRunStore } from "./store/runStore";
import type { MarketSpeed } from "./game/types";

const percent = (value: number) => `${Math.round(value)}%`;

const SPEED_INTERVALS: Record<MarketSpeed, number> = {
  focus: 2400,
  normal: 1400,
  frenzy: 900,
  meltdown: 550,
};

export default function App() {
  const {
    world,
    selectedAction,
    launchTicker,
    launchNarrative,
    setLaunchConfig,
    randomizeLaunch,
    launch,
    setSpeed,
    selectAction,
    performSelectedAction,
    tick,
    resolveCrisis,
    crisisFreeze,
    rugPull,
    pivot,
    catchUp,
  } = useRunStore();

  useEffect(() => { catchUp(); }, []);

  useEffect(() => {
    const baseInterval = SPEED_INTERVALS[world.speed];
    const frenzy = world.activeNarrative?.frenzy;
    const frenzyAccel = frenzy?.active ? frenzy.intensity * 80 : 0;
    const attention = world.activeNarrative?.coin.attention ?? 0;
    const chains = world.activeNarrative?.momentumChains.length ?? 0;
    const delay = Math.max(350, baseInterval - chains * 60 - attention * 2 - frenzyAccel);
    const timer = window.setInterval(tick, delay);
    return () => window.clearInterval(timer);
  }, [world.speed, world.activeNarrative?.frenzy?.active, world.activeNarrative?.frenzy?.intensity, world.activeNarrative?.coin.attention, world.activeNarrative?.momentumChains.length, tick]);

  const narr = world.activeNarrative;
  const coin = narr?.coin;
  const frenzyActive = narr?.frenzy.active ?? false;
  const stressLevel = coin ? Math.max(coin.heat, 100 - coin.trust, coin.volatility, 100 - coin.narrativeStability) : 0;
  const speedClass = world.speed === "focus" ? "speed-focus" : world.speed === "meltdown" ? "speed-meltdown" : "";

  return (
    <main className={`min-h-screen bg-[#05070a] p-4 text-slate-100 md:p-6 ${stressLevel > 72 ? "stress-mode" : ""} ${frenzyActive ? "frenzy-mode" : ""} ${speedClass}`}>
      {frenzyActive && <FrenzyOverlay intensity={narr!.frenzy.intensity} />}

      <header className="mb-5 flex flex-col gap-4 border-b border-cyan-300/10 pb-4 lg:flex-row lg:items-end lg:justify-between">
        <div>
          <div className="flex items-center gap-3 text-xs uppercase tracking-[0.35em] text-cyan-300">
            <span>Pump.exe</span>
            <span className="rounded bg-slate-800 px-2 py-0.5 text-[10px] text-slate-400">{world.season.label}</span>
            {frenzyActive && <span className="animate-pulse rounded bg-pink-500/20 px-2 py-0.5 text-pink-200">FRENZY x{narr!.frenzy.intensity.toFixed(1)}</span>}
          </div>
          {coin ? (
            <h1 className="mt-2 font-mono text-4xl font-black uppercase tracking-tight text-white">
              ${coin.ticker} <span className="text-slate-600">/</span> {coin.narrative}
            </h1>
          ) : (
            <h1 className="mt-2 font-mono text-2xl font-black uppercase tracking-tight text-slate-400">
              no active narrative — launch one below
            </h1>
          )}
        </div>
        <div className="flex flex-wrap items-center gap-3">
          <div className="flex flex-col items-end gap-1">
            <div className="font-mono text-xs text-cyan-200">influence: {Math.round(world.influence)}</div>
            <div className="font-mono text-[10px] text-slate-500">reach: {Math.round(world.reach)} / notoriety: {Math.round(world.notoriety)}</div>
          </div>
          <SpeedControl speed={world.speed} onSpeed={setSpeed} />
        </div>
      </header>

      {coin && narr ? (
        <>
          <section className="mb-5 grid gap-3 sm:grid-cols-2 lg:grid-cols-4 xl:grid-cols-8">
            <MetricCard label="cash" value={formatMoney(world.cash)} detail="operator balance" tone="good" />
            <MetricCard label="price" value={formatMoney(coin.price)} detail="belief per token" />
            <MetricCard label="liquidity" value={formatMoney(coin.liquidity)} detail="available exit meat" />
            <MetricCard label="volume" value={formatMoney(coin.volume)} detail="panic throughput" tone={coin.volume > coin.liquidity * 0.45 ? "warn" : "neutral"} />
            <MetricCard label="hype" value={percent(coin.hype)} detail="narrative oxygen" tone={coin.hype > 70 ? "warn" : "neutral"} />
            <MetricCard label="trust" value={percent(coin.trust)} detail="collapse fuse" tone={coin.trust < 30 ? "danger" : "good"} />
            <MetricCard label="heat" value={percent(coin.heat)} detail="adult supervision" tone={coin.heat > 70 ? "danger" : "neutral"} />
            <MetricCard label="stability" value={percent(coin.narrativeStability)} detail="story coherence" tone={coin.narrativeStability < 35 ? "danger" : "neutral"} />
          </section>

          <div className="grid gap-5 xl:grid-cols-[360px_minmax(0,1fr)_360px]">
            <div className="space-y-5">
              <ActionPanel
                world={world}
                selectedAction={selectedAction}
                onSelect={selectAction}
                onExecute={performSelectedAction}
                onCrisisFreeze={crisisFreeze}
                onPivot={pivot}
                onRug={rugPull}
              />
              <InfluencerPanel influencers={world.influencers} />
            </div>
            <div className="space-y-5">
              <MarketChart points={world.market} frenzyActive={frenzyActive} />
              <MomentumPanel narr={narr} tick={world.tick} />
              <FactionPanel factions={world.factions} />
              <CrisisPanel crises={narr.crises} onResolve={resolveCrisis} />
              <TerminalPanel lines={world.terminal} />
            </div>
            <FeedPanel feed={world.feed} frenzyActive={frenzyActive} />
          </div>
        </>
      ) : (
        <div className="grid gap-5 xl:grid-cols-[minmax(0,1fr)_360px]">
          <div className="space-y-5">
            <LaunchPanel
              ticker={launchTicker}
              narrative={launchNarrative}
              cash={world.cash}
              onConfig={setLaunchConfig}
              onRandomize={randomizeLaunch}
              onLaunch={launch}
            />
            {world.deadNarratives.length > 0 && (
              <section className="panel p-4">
                <h2 className="section-title">ecosystem scars</h2>
                <div className="mt-4 space-y-2">
                  {world.deadNarratives.map((d, i) => (
                    <div key={i} className="flex items-center justify-between rounded border border-red-400/20 bg-red-950/10 p-3 font-mono text-xs">
                      <span className="text-red-200">${d.ticker}</span>
                      <span className="text-slate-500">{d.cause}</span>
                      <span className="text-slate-400">+{Math.round(d.influence)} influence</span>
                    </div>
                  ))}
                </div>
              </section>
            )}
            <FactionPanel factions={world.factions} />
            <TerminalPanel lines={world.terminal} />
          </div>
          <FeedPanel feed={world.feed} frenzyActive={false} />
        </div>
      )}
    </main>
  );
}
