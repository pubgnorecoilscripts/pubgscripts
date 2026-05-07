# Mall Collapse - Prototype Code Status

## What Exists Now

The repository now contains an Unreal Engine 5 C++ project scaffold and the first gameplay primitives for the Mall Collapse first playable.

Project files:

- `MallCollapse.uproject`
- `Source/MallCollapse.Target.cs`
- `Source/MallCollapseEditor.Target.cs`
- `Source/MallCollapse/MallCollapse.Build.cs`

Core runtime systems:

| System | Code | Purpose |
| --- | --- | --- |
| Shared gameplay types | `MallCollapseTypes.h` | Match phases, carry states, hazard types, extraction states, sabotage types, ping types, and loot descriptors. |
| Character shell | `MCCharacter` | Replicated player character with carry, panic, and interaction components. |
| Match flow | `MCGameMode`, `MCGameState` | Server-authoritative four-phase match clock: Entry, Instability, Collapse Cascade, Final Extraction, Match Ended. |
| Match pressure | `MCMatchDirector` | Server-side extraction director that rotates exit availability/failure by match phase. |
| Loot and carry | `MCLootItem`, `MCCarryComponent` | Physical loot pickup/drop/extraction, carried value/weight, visible encumbrance states, movement multipliers. |
| Panic | `MCPanicComponent` | Replicated panic meter with passive recovery, panic tiers, movement stability, and interaction fumble tuning hooks. |
| Hazards | `MCHazardVolume` | Replicated hazard volumes that apply panic, optional damage, and linked mall module damage over time. |
| Extraction | `MCExtractionZone` | Server-authoritative extraction zone that cashes out carried loot, records player extraction results, and hides/disables extracted actors. |
| Mall integrity | `MCMallModuleStateActor` | Replicated module damage model for fire, water, overload stress, electrical instability, and authored integrity states. |
| Sabotage | `MCSabotageDevice` | Interactable replicated sabotage device with cooldown, one-shot support, duration reset, and Blueprint events. |
| Pings | `MCPlayerController`, `MCPingActor` | Server-spawned replicated ping actors for danger, loot, exits, help, suspicion, fake exits, plus view-trace ping creation. |
| Interaction | `MCInteractable`, `MCInteractionComponent` | Lightweight multiplayer interaction interface used by loot and sabotage devices, with server-side view traces. |
| Input hooks | `MCCharacter` | Enhanced Input binding hooks for movement, look, interact, drop loot, and danger ping. |

## How Designers Should Use This

1. Open `MallCollapse.uproject` in Unreal Engine 5.4 or update the engine association to the installed UE5 version.
2. Generate project files from the Unreal context menu or editor prompt.
3. Compile the `MallCollapse` module.
4. Create Blueprint subclasses for:
   - `MCCharacter`
   - `MCLootItem`
   - `MCHazardVolume`
   - `MCExtractionZone`
   - `MCSabotageDevice`
   - `MCMallModuleStateActor`
   - `MCPingActor`
   - `MCMatchDirector`
5. Build a small atrium test map with:
   - 4-6 player starts.
   - Several loot items of different weights/values.
   - Two or more extraction zones for director-controlled exit rotation.
   - One fire/smoke hazard volume linked to a mall module state actor.
   - One sabotage device.
   - One module state actor driving an authored collapse mesh swap in Blueprint.
6. Create Enhanced Input assets and assign them on an `MCCharacter` Blueprint:
   - Move
   - Look
   - Interact
   - Drop Loot
   - Ping

## Current Prototype Limits

This is not a playable packaged build yet. The code establishes the first multiplayer gameplay foundation, but the following work is still required:

- Camera setup.
- Animation Blueprint and carry poses.
- Actual mall map and art assets.
- Blueprint visual/audio responses for hazards, extraction, sabotage, panic, and module state changes.
- Dedicated server build configuration.
- Online session/matchmaking flow.
- UI for carried value, panic, phase, pings, and extraction state.
- Automated UE build/test pipeline.

## Next Coding Targets

Recommended next code tasks:

1. Add a spring-arm/camera setup and first-person/third-person camera mode decision.
2. Add lightweight UI data view models for HUD and post-match summary.
3. Add player health/downed/dragging support so extraction can create rescue/betrayal moments.
4. Add gadget base classes for foam blocker, portable battery, glass cutter, noise decoy, and cart booster.
5. Add hazard-to-extraction interactions, such as fire causing an exit to move from available to failing soon.
6. Add private lobby/session scaffolding for local multiplayer playtests.
