# Pump.exe

A darkly comedic near-future internet capitalism simulator prototype about manufacturing belief faster than reality collapses.

This repository now contains a lean Electron + React + TypeScript foundation for a run-based MVP. The current vertical slice includes:

- deterministic seeded market simulation
- fake social feed
- hype/liquidity/trust/heat/volatility loop
- influencer liabilities that can go rogue
- crisis queue with risk/reward responses
- pivot and rug-pull decisions
- SQLite save seam in Electron main process
- Tailwind-powered fake terminal UI

## Development

```bash
npm install
npm run dev:web
```

For the desktop shell:

```bash
npm run dev
```

## Checks

```bash
npm run typecheck
npm run build
```

## Architecture

- `src/game/*` contains deterministic simulation logic and data.
- `src/store/runStore.ts` adapts the simulation to UI actions with Zustand.
- `src/components/*` renders focused UI panels.
- `electron/*` owns the desktop window and SQLite persistence boundary.

The simulation is intentionally not a realistic finance model. It is tuned around tension: every hype-generating action also increases instability, attention, heat, or trust decay.
