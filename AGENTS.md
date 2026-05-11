# AGENTS.md

## Cursor Cloud specific instructions

This branch (`cursor/pump-mvp-foundation-5bb9`) contains **Pump.exe**, an Electron + React + Vite game.

### Quick reference

| Task | Command |
|------|---------|
| Install deps | `npm install` |
| Typecheck | `npm run typecheck` |
| Build (prod) | `npm run build` |
| Dev server (browser-only) | `npm run dev:web` (serves on `:5173`) |
| Dev w/ Electron | `npm run dev` (launches Electron shell pointing at Vite) |

### Notes for Cloud Agents

- **Use `npm run dev:web`** for development in headless/cloud environments. The full `npm run dev` launches Electron which requires a display server.
- **better-sqlite3** compiles a native addon during `npm install`. The VM already has `g++`, `make`, and `python3` so this works out of the box — no extra system deps needed.
- **No external services** are required. The game runs entirely client-side; SQLite is embedded and only used inside the Electron main process.
- **No lint script** is configured in `package.json`. Use `npm run typecheck` as the primary static analysis check.
- The Vite dev server binds to `0.0.0.0:5173` (configured via `--host 0.0.0.0` in the scripts).
- Build output goes to `dist/` (Vite bundle) and `dist-electron/` (compiled Electron TS).
