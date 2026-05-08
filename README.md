# Mall Collapse

**Mall Collapse** is a multiplayer extraction-chaos game concept for Steam: players loot a gigantic shopping mall during an active structural disaster, sabotage rivals, form temporary alliances, and escape before greed gets them killed.

> Grab what you can carry. Trust nobody for long. Escape before the mall falls apart.

## Product Direction

- **Genre:** Multiplayer extraction-chaos / disaster survival / social sandbox
- **Players:** 6-16 players, with a 4-6 player first playable target
- **Match Length:** 20-30 minutes
- **Engine Target:** Unreal Engine 5
- **Tone:** Dark disaster comedy, stylized realism, commercial apocalypse
- **Business Model:** Paid Steam Early Access with ethical cosmetic monetization

## Design Pillars

1. **The mall is the main enemy** - fire, smoke, flooding, power failure, shutters, and structural collapse create most of the pressure.
2. **Greed creates drama** - loot is physical, visible, and dangerous to carry.
3. **Temporary trust** - cooperation is useful, unstable, and never guaranteed.
4. **Systemic chaos over scripted events** - reusable hazards, gadgets, routes, and sabotage generate emergent stories.
5. **Replayability over content volume** - a small number of deep mall modules should produce many different matches.
6. **Production feasibility matters** - predefined destruction states, modular assets, limited player counts, and dedicated servers keep scope realistic.

## Core Loop

1. Enter through a randomized mall entrance.
2. Loot stores, service spaces, and high-risk premium areas.
3. Manage carry weight, panic, hazards, and social pressure.
4. Cooperate, avoid, bluff, or sabotage other players.
5. Adapt as exits fail and routes collapse.
6. Extract with profits or lose everything in a memorable disaster.

## Documentation

- [Core Game Design](docs/mall-collapse-core-design.md) - full design pillars, match flow, player systems, mall systems, sabotage, UX, retention, and virality.
- [Production and Technical Plan](docs/mall-collapse-production-plan.md) - MVP scope, Early Access path, technical architecture, content priorities, cut lines, and prototype backlog.
- [Prototype Code Status](docs/prototype-code-status.md) - current UE5 C++ scaffold, implemented systems, limits, and next coding targets.

## Code Status

This repo now includes a UE5 C++ project scaffold:

- `MallCollapse.uproject`
- `Source/MallCollapse/`
- `Config/DefaultEngine.ini`
- `Config/DefaultInput.ini`

Implemented first-pass runtime systems include replicated match phases, a match director, high-value loot risk, carry/loot, panic, dynamic hazards, extraction panic events, player extraction results, mall module integrity, social sabotage devices, mall announcements, atmosphere cues, pings, interaction traces, Enhanced Input hooks, and a basic player character/controller shell.

For first-playable execution, the C++ GameMode now spawns an ugly runtime greybox arena with placeholder loot, one hazard, two extraction zones, one sabotage switch, camera support, keyboard/mouse input fallback, visible carried loot, and a debug HUD.

## First Playable Scope

The first playable should prove the core toy before expanding content:

- One mall floor.
- 4-6 players.
- Basic extraction.
- Small/medium/large loot with carry penalties.
- Fire, smoke, and local collapse hazards.
- High-value loot that slows, obstructs, makes noise, and panics greedy players.
- Dynamic mall announcements, fake alerts, sabotage events, and final extraction pressure.
- Simple pings and proximity communication.
- One primary exit and one backup exit.
- Fast post-match results and requeue flow.

## North Star

Mall Collapse should create the feeling:

**"I can survive if I risk a little more."**

Every system should support greed, panic, betrayal, accidental comedy, and dramatic escape stories while remaining understandable, replayable, and feasible for a small indie team.
