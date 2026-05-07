# Mall Collapse - Core Game Design

## Product Promise

**Mall Collapse** is a 6-16 player extraction-chaos game set inside a gigantic shopping mall during an active structural disaster. Players enter unstable retail spaces, loot valuables, sabotage rivals, make temporary alliances, and try to escape before the mall turns their greed into a story.

The commercial hook is simple to understand and highly watchable:

> "Grab what you can carry, trust nobody for long, and escape before the mall falls apart."

## Design Pillars

| Pillar | Practical Meaning | Production Rule |
| --- | --- | --- |
| The mall is the main enemy | Most tension comes from route loss, fire, flooding, power failures, smoke, and collapsing shortcuts. | Build reusable hazard systems before adding content volume. |
| Greed creates drama | Carrying more loot slows, blinds, exposes, and delays players. | Every valuable item must create a movement, visibility, time, or social cost. |
| Temporary trust | Cooperation helps players survive, but betrayal stays tempting. | Avoid permanent teams in the primary mode; use proximity tools, pings, gestures, and shared incentives. |
| Systemic chaos over scripted events | Hazards, loot, players, and exits combine differently each match. | Prefer parameterized systems and authored states over bespoke set pieces. |
| Replayability over content volume | A few strong modules should produce many stories. | Build mall modules with multiple routes, hazard states, locks, and extraction variants. |
| Feasible by a small team | Scope stays modular, stylized, and network-conscious. | Use predefined destruction states, limited simulation, and scalable dedicated servers. |

## Target Experience

### Feel

- Chaotic but readable.
- Tense without being punishingly hardcore.
- Social without requiring voice chat.
- Funny because players make bad decisions under pressure.
- Easy to understand in a clip: overloaded player, collapsing route, exit timer, betrayal.

### Tone

Dark disaster comedy with a "commercial apocalypse" identity:

- Bright retail branding against emergency lighting.
- Cheery PA announcements warped by fire alarms.
- Consumer excess turned into survival liability.
- Semi-serious stakes, not cartoon slapstick.

## Core Loop

1. Spawn at a randomized mall entrance.
2. Scout nearby stores, tools, hazards, and exit routes.
3. Loot valuables with physical carry consequences.
4. Decide whether to cooperate, avoid, threaten, or sabotage other players.
5. React to escalating mall instability.
6. Choose whether to extract safely or risk one more high-value grab.
7. Escape with profits or create a memorable failure.
8. Spend rewards on cosmetics, mastery progress, and loadout variety without stat advantages.

## Match Structure

Target match length: **20-30 minutes**.

| Phase | Time Band | Player Emotion | Systems Emphasized | Design Goal |
| --- | --- | --- | --- | --- |
| 1. Entry | 0-5 min | Curiosity, low pressure | Loot discovery, store scouting, light hazards | Let players form goals and encounter strangers without immediate chaos. |
| 2. Instability | 5-12 min | Rising stress | Alarms, flickering lights, water, early fire, elevators failing | Push players into decisions and unlock more valuable zones. |
| 3. Collapse Cascade | 12-22 min | Panic, greed, betrayal | Route loss, smoke, structural failures, extraction changes | Create the core "I can survive if I risk a little more" tension. |
| 4. Final Extraction | 22-30 min | Desperation | Limited exits, severe hazards, last-minute sabotage | Produce cinematic escapes, revenge, sacrifice, and regret. |

Phase timings should vary by match modifier, player activity, and hazard propagation. The game should telegraph phase changes clearly through audio, lighting, UI, and environmental motion.

## Primary Mode: Collapse Run

### Objective

Extract with the highest personal profit before the mall becomes unsurvivable.

### Win Condition

There is no single winner screen in the battle royale sense. The match resolves with:

- Extracted players and their profit.
- Failed players and their lost potential value.
- Dramatic tags: "Last Exit," "Most Greedy," "Betrayed Ally," "Rescued Rival," "Cart Disaster."

This supports social comparison without forcing esports-first balance.

### Player Count

- First playable: 4-6 players.
- Early Access target: 8-12 players.
- Long-term supported range: 6-16 players.

## Player Systems

### Movement

Movement should be responsive, readable, and slightly desperate under pressure.

Core actions:

- Sprint.
- Crouch.
- Slide under shutters or debris.
- Vault counters, planters, rails, and collapsed fixtures.
- Climb debris piles and service ladders.
- Shimmy across unstable ledges.
- Zipline traversal from tool-created anchors or authored mall routes.

Design guidance:

- Traversal actions should have clear commitment windows so carrying heavy loot or panicking matters.
- Readability beats parkour complexity. Players should know whether a move is possible from a glance.
- Routes should often support two choices: safe but slow, dangerous but short.

### Carry and Inventory

Loot must be physical, visible, and socially legible.

| Carry State | Trigger | Gameplay Effect | Social Effect |
| --- | --- | --- | --- |
| Light | Small valuables, tools, low weight | Full movement and quick interactions | Harder to judge profit; safer for stealth. |
| Encumbered | Medium items or moderate total value | Slight sprint and vault penalties | Looks worth robbing or following. |
| Overloaded | Large items, high stack value | Slower turning, noisier movement, worse climbing | Broadcasts greed and vulnerability. |
| Dragging | Oversized loot or injured player | One hand occupied, poor escape options | Creates cooperation, betrayal, and comedy. |

Example loot:

- Small: jewelry, phones, watches, gift cards, rare sneakers.
- Medium: game consoles, camera kits, designer bags, cash drawers.
- Large: TVs, arcade cabinets, massage chairs, safe boxes.
- Risky: damaged battery packs, leaking propane display, cursed promotional mascot head.

Psychology: visible greed turns value into a social signal. Other players instantly understand why someone is moving slowly and why they are a target.

### Panic System

Panic replaces conventional stamina as the pressure resource.

Panic rises from:

- Nearby collapse impacts.
- Smoke inhalation.
- Darkness.
- Fire proximity.
- Being chased.
- Carrying too much.
- Getting separated from clear exits.
- Loud PA warnings or false evacuation messages.

Panic effects:

- Heavier breathing and louder audio profile.
- Slight interaction fumbles on risky actions.
- Increased reticle sway for thrown tools.
- Reduced balance on unstable surfaces.
- Shorter window to recover from trips or falls.

Panic recovery:

- Reaching lit safe pockets.
- Dropping weight.
- Helping or being helped by another player.
- Using calm items, emergency masks, or mall security rooms.
- Extracting early.

Important: panic should pressure players, not randomly steal control. Failure must feel like a consequence of greed, route choice, or social pressure.

## Mall Systems

### Structural Integrity

Each mall module tracks simplified integrity across a few network-friendly variables:

| Variable | Sources | Results |
| --- | --- | --- |
| Fire damage | Spread from stores, electrical shorts, gas leaks | Smoke, blocked routes, sprinkler activation, collapse risk. |
| Water damage | Sprinklers, flooding, burst pipes | Slippery floors, electrical danger, weakened floors. |
| Overload stress | Player clusters, heavy loot, falling debris, vehicles/carts | Localized floor cracks, railing failure, route denial. |
| Electrical instability | Sabotage, water, generator failures | Light loss, shutter malfunction, escalator reversal, locked doors. |

Use **predefined destruction states**:

1. Intact.
2. Damaged and readable.
3. Hazardous but passable.
4. Partially collapsed.
5. Blocked or destroyed.

This creates dramatic change without full physics simulation.

### Hazard Systems

| Hazard | Gameplay Use | Counterplay | Network/Production Approach |
| --- | --- | --- | --- |
| Fire spread | Area denial, smoke, panic | Sprinklers, foam blockers, alternate routes | Grid/volume propagation with authored burn states. |
| Flooding | Route pressure, electrical danger, object movement | Shut valves, use high routes, reroute power | Zone-based water levels and material swaps. |
| Smoke | Visibility loss, audio emphasis, panic | Masks, fans, thermal scanner, crawl routes | Volumetric zones with cheap gameplay collision volumes. |
| Electrical failures | Darkness, shutters, escalators, doors | Portable battery, breaker rooms, hacking tools | Replicated state changes on authored devices. |
| Structural collapse | Route loss, cinematic danger | Warning tells, shimmy routes, debris climbing | Timed state transitions with debris meshes and nav updates. |
| Security lockdowns | Chokepoints and social traps | Keycards, hacking, shutters forced open | Door/shutter state machines. |

### Hazard Fairness Rules

- Every lethal hazard needs at least one readable tell.
- The first appearance of a hazard should be survivable.
- Unfair deaths become funny only after the player understands what they did wrong.
- Late-match hazards may be severe, but the UI and audio must make exit urgency obvious.

## Sabotage Systems

Sabotage works best when it changes the environment rather than directly killing players.

| Sabotage | Result | Why It Creates Stories | Counterplay |
| --- | --- | --- | --- |
| Reverse escalator | Slows overloaded rivals and breaks escape rhythm | Visible comedy and panic | Use stairs, disable panel, jump off. |
| Fake PA announcement | Misleads players toward bad exits or fake safety | Creates distrust without voice chat | Cross-check map, listen for distortion. |
| Lockdown shutters | Splits groups or traps greedy players in stores | Betrayal is visible and understandable | Force open, hack, find service route. |
| Elevator rerouting | Sends players to unstable floors | Turns convenience into risk | Check status panel, cut power. |
| Flood valve trigger | Adds water and electrical danger to a wing | Creates delayed consequences | Close valve, disable breaker. |
| Gas leak trigger | Makes fire-prone area explosive later | Encourages manipulation and bluffing | Ventilate, avoid sparks, report leak. |
| Power shutdown | Kills lights and locks some devices | Increases fear and stealth play | Portable batteries, breaker rooms. |

Design rule: sabotage should usually create opportunity, not guaranteed elimination. The best sabotage makes the victim say, "I should have seen that coming."

## Gadgets

Gadgets should be simple verbs with broad systemic use.

| Gadget | Primary Use | Secondary Story Potential | Constraints |
| --- | --- | --- | --- |
| Glass cutter | Quietly enter storefronts and display cases | Enables stealth theft while others fight | Slow channel, visible mark. |
| Foam blocker | Temporarily stops fire, smoke, doors, or vents | Heroic rescue or selfish escape denial | Limited charges, decays. |
| Portable battery | Power doors, lights, shutters, scanners | Creates tradeable utility | Heavy enough to matter. |
| Noise decoy | Pull players or security systems | Panic misdirection | Distinct audio signature. |
| Zipline launcher | Create risky shortcuts over atriums | Viral escapes and failures | Requires anchor points, high commitment. |
| Thermal scanner | See heat through smoke and walls | Detect hidden players or fire | Battery drain, narrow cone. |
| Signal jammer | Disrupt pings, maps, fake alerts | Enables betrayal | Short range, obvious interference. |
| Shopping cart booster | Fast transport for loot or players | Clip-friendly disasters | Poor control when overloaded. |

Loadouts should not create hard classes. Players find or select light preferences, then adapt to what the mall gives them.

## Social Chaos Without Mandatory Voice

Supported communication:

- Context pings: loot, danger, exit, help, suspicious player.
- Gestures: point, wave over, stop, surrender, trade.
- Quick chat: "Split?", "Exit here", "Need help", "Drop it", "Trap."
- Emotes with gameplay readability but no hitbox abuse.
- Misleading signals: fake exit pings, decoy help calls, tampered signage.

Trust levers:

- Two-player interactions can be faster, but leave one player exposed.
- Some locked loot is easier with cooperation.
- Dragging injured players can generate rewards, but also makes both vulnerable.
- Extraction doors may allow everyone through, but only if players resist sabotage.

Psychology: players should repeatedly ask, "Are they helping me because it helps them, or because they are setting me up?"

## Map Design

### Modular Mall Approach

Use handcrafted modules assembled into semi-procedural layouts. Each module supports multiple hazard states, loot tables, and route variants.

Core modules:

- Arcade.
- Food court.
- Cinema.
- Luxury wing.
- Maintenance tunnels.
- Department store.
- Parking garage.
- Rooftop.

### Layout Rules

- Every major area needs at least three route categories: obvious, risky, hidden/service.
- Vertical sightlines are valuable: players should see both loot and danger across atriums.
- Chokepoints should be dynamic, not permanent.
- Extraction routes rotate and fail, but must remain readable.
- Players need landmarks: giant signage, brand colors, statues, escalator banks, fountains.

### First Map Recommendation

Build one highly replayable "Central Atrium" mall:

- One main atrium.
- Two retail wings.
- Food court balcony.
- Department store anchor.
- Maintenance backrooms.
- Parking garage extraction.
- Rooftop/service extraction.

This supports verticality, sabotage, loot variety, and clear spectator readability with manageable asset scope.

## Extraction Design

Extraction should be a decision, not a finish line.

Exit types:

- Main doors: obvious, crowded, likely to fail mid-match.
- Parking garage ramps: loot-friendly, fire/flood-prone.
- Rooftop evacuation: high-risk vertical route, cinematic.
- Service tunnels: hidden, low capacity, confusing.
- Emergency shutters: require power, battery, or cooperation.

Extraction modifiers:

- Capacity limits.
- Temporary opening windows.
- Hazard-dependent failures.
- Noise that attracts players.
- Two-person overrides that can be betrayed.

## Progression

Progression should deepen identity without selling power.

### Mastery Tracks

| Track | Progression Fantasy | Example Challenges | Rewards |
| --- | --- | --- | --- |
| Scavenger | Efficient greed | Extract with compact high-value loot | Outfits, bags, loot tags. |
| Saboteur | Environmental manipulation | Cause rivals to reroute or drop loot | Gloves, tools, emotes. |
| Rescue Specialist | Risky cooperation | Drag injured players to safety | Jackets, badges, heroic titles. |
| Stealth Looter | Quiet opportunist | Extract unseen from premium stores | Masks, dark cosmetics. |
| Systems Hacker | Mall control expert | Restore or redirect power systems | Device skins, UI flair. |

### Account Reputation

Reputation should be descriptive rather than punitive:

- "Reliable Helper."
- "Known Backstabber."
- "Exit Camper."
- "Mall Hero."
- "Loot Goblin."

These labels can be opt-in/profile-facing or post-match flavor. Avoid systems that permanently punish social chaos.

## Retention and Live Ops

Lightweight daily/weekly systems:

- Featured stores with boosted loot themes.
- Rotating hazard emphasis: smoke-heavy, blackout, flood surge.
- Cosmetic objectives: escape wearing mascot head, extract three luxury items, rescue a rival.
- Daily mall rumor: "Jewelry wing shutters are malfunctioning."

Avoid chore design:

- No mandatory daily streak power.
- No stat boosts.
- No long grind gates for essential tools.

Long-term event examples:

- Flood Week.
- Total Blackout.
- Holiday Rush.
- Fire Sale.
- Winter Mall.
- Luxury Expo.
- Dead Mall.
- Tech Convention.
- Black Friday Event.

## Streamer and Viral Design

The game should reliably produce clips where viewers instantly understand:

- Who is greedy.
- What is collapsing.
- Where the exit is.
- Why betrayal happened.
- Why the failure was funny.

Clip generators:

- Overloaded players trying to jump collapsing gaps.
- Shopping cart booster pileups.
- PA fakeout betrayals.
- Last-second shutter closures.
- Zipline escapes over burning atriums.
- Players arguing through pings and gestures.
- Rival rescue turning into mutual extraction.

UX support:

- Post-match highlight tags.
- Spectator-friendly danger outlines.
- Clear extracted/lost value numbers.
- Saveable replay moments in Early Access roadmap.

## UX Priorities

### HUD

Keep the HUD readable under chaos:

- Current carried value.
- Encumbrance state.
- Panic level.
- Known exits and confidence state: open, unstable, unknown, failed.
- Immediate hazard warnings.
- Gadget slots.

### Diegetic Information

Use the mall itself as UI:

- PA announcements.
- Emergency signage.
- Store shutters.
- Flickering lights.
- Sprinklers and water flow.
- Smoke color and density.
- Floor cracks and dust falls.

### Onboarding

The tutorial should teach the fantasy quickly:

1. Grab small loot.
2. Grab oversized loot and feel the downside.
3. Use a gadget to open a route.
4. Hear a hazard warning.
5. Watch an exit fail.
6. Escape early with modest profit.
7. Replay with the temptation to stay longer.

## Commercial Positioning

Mall Collapse sits between:

- Extraction games.
- Disaster survival games.
- Social chaos games.
- Emergent sandbox games.

Differentiators:

- Environment-first antagonist.
- Greed expressed physically.
- Temporary trust without mandatory voice chat.
- Procedural disaster pressure.
- Dark retail comedy.
- Production-feasible destruction.

## Design Risks and Mitigations

| Risk | Why It Matters | Mitigation |
| --- | --- | --- |
| Chaos becomes unreadable | Players blame the game instead of their decisions | Strong audio tells, signage, hazard colors, and phase clarity. |
| Sabotage feels griefy | New players quit if they feel helpless | Require setup, telegraph effects, add counterplay and post-match clarity. |
| Extraction too safe | The "one more round" loop weakens | Make late loot tempting and exits dynamic. |
| Extraction too punishing | Losses feel frustrating | Preserve partial rewards, dramatic summaries, and fast requeue. |
| Scope balloons | Small-team production fails | Limit first playable to one floor, few hazards, few gadgets, one mode. |
| Voice chat dependency | Silent players miss social gameplay | Pings, gestures, quick chat, visible carry states, environmental signals. |

## Golden Rules for New Features

Every feature must answer yes to most of these:

1. Is it immediately understandable?
2. Does it create player stories?
3. Does it increase greed, fear, trust, or escape tension?
4. Can it interact with existing hazards, loot, or routes?
5. Can it be built with modular assets and replicated state?
6. Does it avoid pay-to-win?
7. Does it improve replayability more than it increases maintenance cost?
