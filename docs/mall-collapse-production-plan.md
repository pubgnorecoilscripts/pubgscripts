# Mall Collapse - Production and Technical Plan

## Goal

This plan turns the Mall Collapse concept into an indie-feasible multiplayer product path for Steam Early Access. It prioritizes a polished, replayable core loop before content volume.

## Product Strategy

### Commercial Thesis

Mall Collapse can stand out on Steam because it combines three proven player motivations in a fresh setting:

- **Extraction tension:** "Do I leave now or risk more?"
- **Social chaos:** "Can I trust this stranger for the next two minutes?"
- **Disaster spectacle:** "The map itself is changing faster than my plan."

The mall theme gives the game instant readability, comedic contrast, and broad asset variety without requiring military realism, large open worlds, or expensive destruction tech.

### Target Audience

- Steam players who enjoy extraction games but want less milsim pressure.
- Groups who play chaotic co-op/versus games and want emergent betrayal.
- Streamers seeking readable disasters, social tension, and highlight moments.
- Indie multiplayer players who value replayability over progression grind.

### Business Model

Recommended launch model:

- Paid Early Access base game.
- Ethical cosmetic DLC or seasonal cosmetic pass after retention is proven.
- No gameplay power sales.

Cosmetic categories:

- Outfits.
- Backpacks.
- Shopping cart skins.
- Gadget skins.
- Emotes.
- Announcer voices.
- Profile badges.
- Extraction banners.

## Development Principles

| Principle | Decision |
| --- | --- |
| Build the toy first | Carrying loot, escaping hazards, and betraying players must be fun before progression exists. |
| Replicate states, not chaos | Network authored hazard state changes rather than full physics. |
| Fewer modules, deeper variants | One great atrium beats five shallow maps. |
| Cut ranked mode | Early retention should come from stories, not ladder pressure. |
| Make losses entertaining | Post-match summaries, partial rewards, and fast requeue reduce frustration. |
| Keep tools systemic | Gadgets should interact with doors, lights, smoke, routes, and players in reusable ways. |

## Recommended Technology

### Engine

Unreal Engine 5.

Rationale:

- Strong multiplayer replication support.
- Good art pipeline for modular environments.
- Niagara, audio, lighting, and level streaming support disaster readability.
- Blueprint enables technical designers to iterate on hazards and gadgets quickly.

### Multiplayer Architecture

Recommended:

- Dedicated authoritative servers.
- Server-authoritative inventory, extraction, hazard states, sabotage, and damage.
- Client prediction only for local movement where appropriate.
- Player count scoped to 6-16 to preserve network budget and readability.

Avoid:

- Peer-to-peer authority for competitive extraction.
- Full rigid-body destruction replication.
- MMO-style persistence.

### Data-Driven Systems

Author designers should be able to tune:

- Loot value, weight, size, and carry pose.
- Hazard spread speed and state transitions.
- Extraction state and failure probability.
- Gadget effects and charges.
- Module spawn tables.
- Phase timing ranges.

Use Unreal data assets/data tables so designers can iterate without code changes.

## MVP Definitions

### First Playable

Purpose: prove that the core fantasy is fun with real players.

Scope:

- One mall floor.
- 4-6 players.
- One primary extraction route and one backup route.
- Simple loot: small, medium, large.
- Carry states: light, encumbered, overloaded.
- Basic movement: sprint, crouch, vault, climb debris.
- Hazards: fire zones, smoke zones, localized collapse blockers.
- Simple VOIP or proximity communication placeholder.
- Basic pings: danger, loot, exit, help.
- End screen: extracted value and failed value.

Cut from first playable:

- Progression.
- Cosmetics.
- Matchmaking polish.
- Multiple maps.
- Complex flooding.
- Advanced sabotage.
- Ranked mode.
- Seasonal content.

Success criteria:

- Players voluntarily risk staying longer after they understand extraction.
- Carrying large loot creates visible comedy and vulnerability.
- Hazards force route changes without feeling random.
- At least one betrayal or temporary alliance happens in most playtests.

### Vertical Slice

Purpose: prove the Steam trailer and Early Access promise.

Scope:

- Central Atrium map with 4-5 connected modules.
- 8-10 players.
- Full four-phase match flow.
- Dynamic extraction rotation/failure.
- Core hazards: fire, smoke, electrical failures, shutters, localized collapse.
- First sabotage set: shutters, escalators, fake PA, power shutdown.
- First gadget set: glass cutter, foam blocker, portable battery, noise decoy, shopping cart booster.
- Pings, quick chat, gestures.
- Post-match awards and highlight tags.
- Basic cosmetics.
- Server browser or lightweight matchmaking.

Success criteria:

- Trailer footage communicates the game in seconds.
- Stream viewers can follow danger, greed, and escape stakes.
- Repeated playtests produce different stories on the same map.
- New players understand how they died or why they escaped.

### Early Access Launch

Purpose: sell a polished replayable foundation and grow with community feedback.

Scope:

- Collapse Run primary mode.
- 8-12 player default queue, configurable private lobbies.
- 2-3 mall layouts assembled from shared modules.
- 6-8 hazard variants/modifiers.
- 8-10 gadgets with clear counterplay.
- Mastery tracks and cosmetic unlocks.
- Daily featured store/modifier system.
- Anti-grief protections for onboarding.
- Dedicated server deployment pipeline.
- Crash reporting, telemetry, moderation basics.

Cut or defer:

- Ranked mode.
- Full replay editor.
- Large-scale procedural generation.
- NPC crowds.
- Full destruction simulation.
- Crafting.
- Deep economy.
- Multiple character classes.

## Technical System Breakdown

### Match Director

Responsibilities:

- Own match phase transitions.
- Select hazard pressure profile.
- Activate high-value areas.
- Rotate/fail extractions.
- Trigger PA announcements.
- Keep late match tense without hard battle royale circles.

Implementation approach:

- Server-authoritative subsystem.
- Uses seeded randomness for reproducibility.
- Pulls from authored event pools.
- Tracks player distribution, extracted value, active exits, and hazard severity.

Key rule:

- The director should pressure, not solve. It creates dilemmas but should not target individual players unfairly.

### Mall Module State

Each module should expose:

- Integrity state.
- Fire state.
- Smoke state.
- Water state.
- Power state.
- Security state.
- Loot availability.
- Active routes.

Implementation approach:

- Modular level chunks with replicated state machines.
- Visual/audio changes attached to state transitions.
- Navigation and traversal updates handled through predefined route toggles.

### Loot and Carry

Core components:

- Loot definition: value, size, weight, noise, carry socket, two-hand requirement.
- Inventory/carry component: current items, carried volume, encumbrance tier.
- Drop/extract component: validates ownership and extraction.

Important design rule:

- The most valuable items should rarely be the most efficient. Players should debate whether a giant TV is worth the escape cost.

### Panic

Core variables:

- Current panic.
- Panic sources.
- Recovery modifiers.
- Encumbrance interaction.
- Hazard proximity modifiers.

Network approach:

- Server validates gameplay-affecting panic thresholds.
- Client handles local camera/audio feedback for responsiveness.

Avoid:

- Random input loss.
- Excessive camera shake.
- Effects that cause motion sickness.

### Sabotage Devices

Every sabotage device should share a predictable structure:

- Interact point.
- Required tool or channel time.
- Telegraph state.
- Result state.
- Counterplay interaction.
- Cooldown or one-shot limit.
- Attribution for post-match stories.

This keeps future sabotage additions cheap.

### Extraction System

Extraction states:

- Unknown.
- Available.
- Temporarily blocked.
- Requires power/tool/cooperation.
- Failing soon.
- Failed.

Extraction should:

- Broadcast enough information to create movement.
- Create crowding and betrayal opportunities.
- Preserve multiple viable late-match paths until the finale.

### Telemetry

Track from the start:

- Extract rate.
- Average carried value at extraction/death.
- Most common death/hazard causes.
- Time spent before first extraction attempt.
- Sabotage usage and success.
- Disconnect rate.
- Requeue rate.
- Exit failures and player proximity.
- New player first-match survival and confusion points.

Use telemetry to tune frustration, not to optimize away chaos.

## UX and Onboarding Plan

### First-Time Experience

Use a short playable scenario:

- Spawn in a closed mall training wing.
- Pick up small loot and see value increase.
- Pick up a TV and feel slowdown.
- Drop loot to escape a hazard.
- Use a ping.
- See an exit change state.
- Extract.

Do not overexplain every system before players have a reason to care.

### Match Clarity

Players should always understand:

- What they are carrying.
- How greedy they are being.
- Which exits are likely.
- What immediate hazard is threatening them.
- Whether a route changed because of collapse, power, or sabotage.

### Post-Match Screen

Show:

- Extracted cash.
- Lost cash.
- Biggest greed mistake.
- Best escape.
- Sabotage caused/suffered.
- Rescue or betrayal moments.
- Mastery progress.
- Fast requeue.

This turns failure into narrative fuel.

## Content Plan

### First Environment Modules

| Module | Primary Gameplay | Reuse Value |
| --- | --- | --- |
| Central Atrium | Sightlines, vertical routes, collapse spectacle | Anchor for trailers and repeated layouts. |
| Food Court | Fire, gas, crowding, slippery floors | Strong hazard variety from shared props. |
| Department Store | Dense loot, maze routes, shutters | High replayability and stealth. |
| Arcade | Noise, power, visual clutter, gadget loot | Comedy and streamer readability. |
| Maintenance Tunnels | Hidden routes, valves, breakers | Supports counterplay and map mastery. |
| Parking Garage | Extraction, carts, flooding, vehicle obstacles | Clear endgame pressure. |

### First Gadget Set

Prioritize gadgets that combine with many systems:

1. Foam blocker.
2. Portable battery.
3. Glass cutter.
4. Noise decoy.
5. Shopping cart booster.

Defer until core loop is proven:

- Zipline launcher.
- Thermal scanner.
- Signal jammer.

### First Sabotage Set

Prioritize readable, reversible sabotage:

1. Lockdown shutters.
2. Fake PA announcement.
3. Escalator reverse.
4. Power shutdown.

Defer:

- Gas leak chaining.
- Elevator rerouting.
- Complex flooding triggers.

## Art Direction Execution

### Visual Target

"Commercial apocalypse":

- Saturated store identities.
- Emergency red/orange lighting.
- Flickering fluorescent strips.
- Smoky silhouettes.
- Neon reflections on wet tile.
- Exaggerated signage for navigation.

### Production Approach

- Stylized realism over photorealism.
- Modular storefront kit.
- Shared destruction overlays: cracked, burned, flooded, powered-off.
- Reusable prop families: retail shelves, mannequins, displays, food court furniture.
- Strong silhouettes for player carry states.

### Readability Rules

- Hazards must have distinct color/shape language.
- Loot categories should be readable from medium distance.
- Exit signage should remain legible under lighting changes.
- Heavy carried items should be funny and identifiable.

## Audio Direction Execution

Audio is a gameplay system, not just atmosphere.

### Audio Layers

- PA announcements.
- Fire alarms.
- Sprinklers.
- Electrical buzzing.
- Escalator grinding.
- Distant collapse rumbles.
- Store music warping under power failure.
- Smoke/fire proximity cues.
- Player breathing and panic.
- Heavy loot impacts and cart noise.

### Design Rules

- Audio tells should precede major route changes.
- Fake PA sabotage must sound subtly different after players learn the game.
- Overloaded players should create useful sound for hunters and helpers.
- Final extraction audio should be iconic and trailer-friendly.

## Retention Plan

### Short-Term

- Fast requeue.
- Post-match awards.
- Daily featured store.
- Rotating hazard modifier.
- Cosmetic objectives that encourage funny behavior.

### Medium-Term

- Mastery tracks.
- New gadgets.
- New store modules.
- Limited-time event rules.
- Private lobby modifiers.

### Long-Term

- Seasonal mall variants.
- New announcer packs.
- Themed events.
- Community challenge weekends.
- Optional replay/highlight export.

Avoid retention traps:

- Mandatory streaks.
- Power grind.
- Excessive currencies.
- Gameplay advantages from purchases.

## Steam Page Positioning

### Short Description

Loot a collapsing shopping mall, sabotage rival survivors, and escape before greed brings the ceiling down.

### Tag Targets

- Multiplayer.
- Extraction.
- Survival.
- PvPvE.
- Co-op.
- Funny.
- Action.
- Simulation.
- Indie.
- Online Co-Op.

### Trailer Beat Sheet

1. Bright mall opening shot, then lights flicker.
2. Player grabs too much loot.
3. Rival offers help through ping/gesture.
4. Fire alarm and shutter slam.
5. Shopping cart escape attempt.
6. Atrium floor collapses.
7. Fake PA sends group the wrong way.
8. One player drops TV to sprint.
9. Last rooftop extraction.
10. End card: "Mall Collapse - Grab. Betray. Escape."

## Quality Bar

Before public Early Access, the game needs:

- Stable dedicated server sessions.
- Clear extraction loop.
- Readable hazards.
- Satisfying movement under encumbrance.
- Working pings/quick chat.
- Fast restart flow.
- Good performance in smoke/fire-heavy scenes.
- Enough map variation to support repeated sessions.
- Moderation and reporting basics.
- No pay-to-win systems.

## Cut Line

If production pressure rises, protect these:

1. Carrying valuable loot feels risky and visible.
2. The mall changes routes and hazards during the match.
3. Players can cooperate and betray without voice.
4. Extraction timing creates regret.
5. Failures are readable and funny.

Cut these first:

1. Extra casual modes.
2. Complex seasonal meta.
3. Advanced gadgets.
4. Additional maps.
5. Replay editor.
6. Ranked mode.
7. NPC systems.
8. Complex procedural generation.

## Next Build Backlog

### Prototype Tasks

- Implement third-person or first-person controller with sprint, crouch, vault, and carry poses.
- Implement loot definitions with value, weight, size, pickup/drop/extract.
- Implement encumbrance tiers that affect movement and interaction.
- Implement one extraction volume and end screen.
- Implement server-authoritative match timer and phase state.
- Implement fire/smoke hazard volumes with readable tells.
- Implement one route collapse state change.
- Implement danger/loot/exit/help pings.
- Implement a small atrium test map with one store and one service route.

### Playtest Questions

- Do players understand that staying longer is optional?
- Do players laugh when greed slows someone down?
- Do hazards redirect players instead of simply killing them?
- Do silent players still coordinate or mislead each other?
- Is extraction failure dramatic rather than confusing?
- Does the same map produce different stories after several rounds?
