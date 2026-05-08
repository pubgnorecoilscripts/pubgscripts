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
| Shared gameplay types | `MallCollapseTypes.h` | Match phases, carry states, high-value loot presets, hazard types, extraction states/events, announcement types, atmosphere cues, sabotage types, ping types, and loot descriptors. |
| Character shell | `MCCharacter` | Replicated player character with carry, panic, and interaction components. |
| Match flow | `MCGameMode`, `MCGameState` | Server-authoritative four-phase match clock: Entry, Instability, Collapse Cascade, Final Extraction, Match Ended. |
| Match pressure | `MCMatchDirector` | Server-side extraction director that rotates exit availability/failure by match phase, emits procedural announcements, triggers hazard events, and escalates final extraction. |
| Loot and carry | `MCLootItem`, `MCCarryComponent` | Physical loot pickup/drop/extraction, high-value loot presets, carried value/weight, visible encumbrance states, movement penalties, noise risk, vision obstruction, and panic buildup. |
| Panic | `MCPanicComponent` | Replicated panic meter with passive recovery, panic tiers, movement stability, and interaction fumble tuning hooks. |
| Hazards | `MCHazardVolume` | Replicated hazard volumes that apply panic, optional damage, linked mall module damage, and timed hazard activations. |
| Extraction | `MCExtractionZone` | Server-authoritative extraction zone with optional countdown delays, panic events, carried loot cash-out, player extraction records, and extracted actor handling. |
| Mall integrity | `MCMallModuleStateActor` | Replicated module damage model for fire, water, overload stress, electrical instability, and authored integrity states. |
| Sabotage | `MCSabotageDevice` | Interactable replicated sabotage device with cooldown, one-shot support, linked extraction/hazard/module effects, duration reset, and Blueprint events. |
| Pings | `MCPlayerController`, `MCPingActor` | Server-spawned replicated ping actors for danger, loot, exits, help, suspicion, fake exits, plus view-trace ping creation. |
| Interaction | `MCInteractable`, `MCInteractionComponent` | Lightweight multiplayer interaction interface used by loot and sabotage devices, with server-side view traces. |
| Input hooks | `MCCharacter` | Enhanced Input binding hooks for movement, look, interact, drop loot, and danger ping. |
| Announcements and atmosphere | `MCGameState` | Replicated mall announcement and atmosphere cue channels for PA lines, fake alerts, sirens, blackouts, shakes, and collapse cues. |
| First playable shell | `MCGreyboxArena`, `MCDebugHUD`, `MCGameMode` | Source-only runtime greybox arena, placeholder loot/hazard/extraction/sabotage spawning, legacy input fallback, camera, carry attach point, and debug HUD. |

## First Fun Layer

The prototype now includes the first systems intended to create greed, betrayal, panic, funny accidents, and emergent multiplayer stories:

- **High-value loot risk:** `MCLootItem::ApplyHighValuePreset` supports giant television, arcade machine, jewelry case, luxury electronics, and safe box presets. These tune value, weight, two-hand carry, movement penalty, vision obstruction, carried noise, and panic pressure.
- **Extraction panic:** `MCExtractionZone` can delay extraction through countdowns and trigger elevator delays, fake extraction alerts, emergency reroutes, collapsing paths, and last-second countdown events.
- **Procedural mall announcements:** `MCMatchDirector` uses contextual announcement pools and `MCGameState::BroadcastMallAnnouncement` for emergency warnings, fake evacuation notices, security alerts, promotions, malfunctioning AI lines, and extraction updates.
- **Social sabotage:** `MCSabotageDevice` now applies concrete linked effects for shutter lockdowns, breaker blackouts, escalator shutdowns, sprinkler flooding, and false emergency alarms.
- **Dynamic hazards:** `MCMatchDirector` can activate linked `MCHazardVolume` targets as fire, smoke, flood, electrical, or structural collapse events and apply module damage.
- **Final extraction escalation:** final phase starts with constrained exits, then periodically narrows available exits and intensifies active hazards/alarms.
- **Atmosphere cues:** `MCGameState::BroadcastAtmosphereCue` replicates lightweight cues for flickering lights, distant collapses, sirens, shakes, environmental panic, and blackouts.

## How Designers Should Use This

1. Open `MallCollapse.uproject` in Unreal Engine 5.4 or update the engine association to the installed UE5 version.
2. Generate project files from the Unreal context menu or editor prompt.
3. Compile the `MallCollapse` module.
4. Press Play in an empty/default map. `MCGameMode` now spawns an ugly runtime greybox arena automatically for first-playable testing.
5. Optional: create Blueprint subclasses for:
   - `MCCharacter`
   - `MCLootItem`
   - `MCHazardVolume`
   - `MCExtractionZone`
   - `MCSabotageDevice`
   - `MCMallModuleStateActor`
   - `MCPingActor`
   - `MCMatchDirector`
6. If replacing the runtime arena, build a small atrium test map with:
   - 4-6 player starts.
   - Several loot items of different weights/values.
   - Two or more extraction zones for director-controlled exit rotation.
   - One fire/smoke hazard volume linked to a mall module state actor.
   - Several high-value loot items using the built-in presets.
   - One sabotage device linked to at least one extraction zone, hazard volume, or mall module.
   - One module state actor driving an authored collapse mesh swap in Blueprint.
7. Enhanced Input assets are optional for now. The C++ character has legacy keyboard/mouse bindings:
   - WASD move
   - Mouse look
   - E interact
   - Q drop loot
   - Middle mouse ping
   - Space jump
8. If using Enhanced Input, create assets and assign them on an `MCCharacter` Blueprint:
   - Move
   - Look
   - Interact
   - Drop Loot
   - Ping

## Current Prototype Limits

This is not a playable packaged build yet. The code establishes the first multiplayer gameplay foundation, but the following work is still required:

- Full authored map. The current arena is runtime-generated C++ greybox.
- Animation Blueprint and carry poses.
- Actual mall art assets.
- Blueprint visual/audio responses for hazards, extraction, sabotage, panic, and module state changes.
- Blueprint visual/audio responses for announcement and atmosphere cues.
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
5. Add local audio/visual Blueprint implementations for loot noise pulses, fake PA lines, blackouts, sirens, and collapse shakes.
6. Add private lobby/session scaffolding for local multiplayer playtests.
