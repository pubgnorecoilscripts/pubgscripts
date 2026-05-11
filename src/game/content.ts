import type { ActionDefinition, Crisis, CrisisChoice, CrisisId, Influencer, Sentiment } from "./types";

export const actionDefinitions: ActionDefinition[] = [
  {
    id: "botBurst",
    label: "Bot Burst",
    command: "deploy 12k reply goblins",
    description: "Cheap engagement spike. Great for ignition, radioactive if overused.",
    cost: 450,
    cooldown: 2,
    effects: { hype: 13, attention: 16, liquidity: 260, heat: 7, trust: -2, volatility: 4 },
  },
  {
    id: "sponsorInfluencer",
    label: "Sponsor Influencer",
    command: "wire an AI alpha prophet",
    description: "Buys reach and legitimacy until the influencer develops opinions.",
    cost: 1_200,
    cooldown: 4,
    effects: { hype: 19, attention: 18, liquidity: 900, heat: 5, trust: 5, volatility: 6 },
  },
  {
    id: "leakRoadmap",
    label: "Leak Roadmap",
    command: "announce impossible roadmap",
    description: "Narrative oxygen. Trust rises now, expectations become a hostage.",
    cost: 750,
    cooldown: 3,
    effects: { hype: 12, attention: 8, liquidity: 560, heat: 3, trust: 10, volatility: 3 },
  },
  {
    id: "manufactureOutrage",
    label: "Manufacture Outrage",
    command: "make everyone mad on purpose",
    description: "Turns hate-clicks into liquidity. Also summons adults with clipboards.",
    cost: 900,
    cooldown: 3,
    effects: { hype: 18, attention: 24, liquidity: 430, heat: 13, trust: -8, volatility: 10 },
  },
  {
    id: "liquidityMirage",
    label: "Liquidity Mirage",
    command: "wash-trade confidence hologram",
    description: "Makes the pool look safer than it is. Whales notice the smell.",
    cost: 1_700,
    cooldown: 5,
    effects: { hype: 9, attention: 5, liquidity: 1_900, heat: 11, trust: -5, volatility: 14 },
  },
];

export const influencerTemplates: Omit<Influencer, "id">[] = [
  {
    handle: "@YieldGremlin",
    archetype: "goblin quant",
    reach: 64,
    credibility: 31,
    alignment: 68,
    volatility: 76,
    status: "loyal",
  },
  {
    handle: "@MommyMiladyAI",
    archetype: "synthetic lifestyle cultist",
    reach: 82,
    credibility: 24,
    alignment: 52,
    volatility: 88,
    status: "loyal",
  },
  {
    handle: "@BasedCompliance",
    archetype: "ex-regulator reply guy",
    reach: 38,
    credibility: 72,
    alignment: 61,
    volatility: 42,
    status: "loyal",
  },
  {
    handle: "@ChartCrimes",
    archetype: "technical analysis arsonist",
    reach: 59,
    credibility: 43,
    alignment: 73,
    volatility: 67,
    status: "loyal",
  },
  {
    handle: "@SlopOracle",
    archetype: "AI prophecy account",
    reach: 71,
    credibility: 18,
    alignment: 80,
    volatility: 93,
    status: "loyal",
  },
];

export const narrativeSeeds = [
  "sentient vending machine DAO",
  "AI-generated pet afterlife protocol",
  "zero-calorie metaverse beef",
  "privacy coin for divorced dads",
  "decentralized apology marketplace",
  "GPU-backed astrology engine",
  "carbon-negative rage bait index",
];

export const tickerSeeds = ["GRIFT", "SLOP", "YELL", "COPE", "FOMO", "RUG", "VIBE", "HACK"];

export const postFragments: Record<Sentiment, string[]> = {
  bullish: [
    "I don't know what this is but the chart has religious implications.",
    "Early wallets are accumulating like raccoons in a data center.",
    "This narrative is so stupid it wrapped around to genius.",
    "If you missed the last scam, congratulations, here's another chance.",
  ],
  bearish: [
    "Founder wallet moved and everyone is pretending that's normal.",
    "The whitepaper is just a screenshot of a Discord argument.",
    "Liquidity looks thinner than a launch-day apology.",
    "I have seen rug pulls with better table manners.",
  ],
  unhinged: [
    "MY GROUP CHAT IS FORMING A GOVERNANCE MILITIA.",
    "THE MASCOT SPOKE TO ME THROUGH A CAPTCHA.",
    "SELLING MY CHAIR. BUYING MORE.",
    "THIS IS NOT FINANCIAL ADVICE, THIS IS A CRY FOR HELP.",
  ],
  regulatory: [
    "Local agency requests clarification regarding 'weaponized vibes'.",
    "Compliance analysts unable to distinguish satire from securities fraud.",
    "A senator's nephew bought the top and now hearings are possible.",
    "Banking partner flags transaction memo: 'for goblin liquidity'.",
  ],
  bot: [
    "Great project ser. Utility soon. Community strong. Very organic.",
    "I am human investor and I enjoy this roadmap sincerely.",
    "Undervalued gem detected by proprietary emotion algorithm.",
    "The devs are cooking. I can smell fiduciary excellence.",
  ],
};

const crisisChoices = {
  deny: {
    id: "deny",
    label: "Deny Everything",
    description: "Fast, cheap, insulting. Works until screenshots breed.",
    effects: { hype: 5, trust: -8, heat: 6, attention: 8, volatility: 6 },
  },
  sacrifice: {
    id: "sacrifice",
    label: "Sacrifice a Fall Guy",
    description: "Blame a contractor, bot, intern, or prophecy engine.",
    effects: { trust: 4, heat: -7, attention: 3, cash: -900, volatility: -3 },
  },
  embrace: {
    id: "embrace",
    label: "Make It Lore",
    description: "Turn failure into canon. Dangerous. Very streamable.",
    effects: { hype: 12, trust: -5, heat: 4, attention: 14, volatility: 10 },
  },
} satisfies Record<string, CrisisChoice>;

export const makeCrisis = (type: CrisisId, id: string, tick: number, severity: number): Crisis => {
  const base = {
    id,
    type,
    severity,
    expiresAt: tick + 4,
    choices: [crisisChoices.deny, crisisChoices.sacrifice, crisisChoices.embrace],
  };

  switch (type) {
    case "regulatorPing":
      return {
        ...base,
        title: "Regulator Ping",
        body: "A bored agency intern found your meme coin in a hearing prep deck.",
      };
    case "rogueInfluencer":
      return {
        ...base,
        title: "Influencer Goes Rogue",
        body: "A sponsored account is improvising theology and threatening to launch a fork.",
      };
    case "whaleDump":
      return {
        ...base,
        title: "Whale Dump",
        body: "A whale wallet starts unloading into your perfectly manufactured belief event.",
      };
    case "fakeHack":
      return {
        ...base,
        title: "Fake Hack Becomes Real",
        body: "Your fake hack stunt attracted someone who can actually type.",
      };
    case "outrageBackfire":
      return {
        ...base,
        title: "Outrage Backfire",
        body: "The hate campaign worked, then learned your home address and tax posture.",
      };
  }
};
