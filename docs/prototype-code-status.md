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
| Loot and carry | `MCLootItem`, `MCCarryComponent` | Physical loot pickup/drop/extraction, carried value/weight, visible encumbrance states, movement multipliers. |
| Panic | `MCPanicComponent` | Replicated panic meter with passive recovery, panic tiers, movement stability, and interaction fumble tuning hooks. |
| Hazards | `MCHazardVolume` | Replicated hazard volumes that apply panic and optional damage over time. |
| Extraction | `MCExtractionZone` | Server-authoritative extraction zone that cashes out carried loot and hides/disables extracted actors. |
| Mall integrity | `MCMallModuleStateActor` | Replicated module damage model for fire, water, overload stress, electrical instability, and authored integrity states. |
| Sabotage | `MCSabotageDevice` | Interactable replicated sabotage device with cooldown, one-shot support, duration reset, and Blueprint events. |
| Pings | `MCPlayerController`, `MCPingActor` | Server-spawned replicated ping actors for danger, loot, exits, help, suspicion, and fake exits. |
| Interaction | `MCInteractable`, `MCInteractionComponent` | Lightweight multiplayer interaction interface used by loot and sabotage devices. |

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
5. Build a small atrium test map with:
   - 4-6 player starts.
   - Several loot items of different weights/values.
   - One available extraction zone.
   - One fire/smoke hazard volume.
   - One sabotage device.
   - One module state actor driving an authored collapse mesh swap in Blueprint.

## Current Prototype Limits

This is not a playable packaged build yet. The code establishes the first multiplayer gameplay foundation, but the following work is still required:

- Input bindings and camera setup.
- Animation Blueprint and carry poses.
- Actual mall map and art assets.
- Blueprint visual/audio responses for hazards, extraction, sabotage, panic, and module state changes.
- Dedicated server build configuration.
- Online session/matchmaking flow.
- UI for carried value, panic, phase, pings, and extraction state.
- Automated UE build/test pipeline.

## Next Coding Targets

Recommended next code tasks:

1. Add Enhanced Input actions for movement, interact, drop loot, ping, and quick chat.
2. Implement line/sphere trace targeting for interaction and pings.
3. Add replicated player extraction results to the GameState.
4. Add extraction failure rotation controlled by a match director actor.
5. Add hazard-to-module damage links so fire/smoke/flood volumes can change mall integrity.
6. Add lightweight UI data view models for HUD and post-match summary.
