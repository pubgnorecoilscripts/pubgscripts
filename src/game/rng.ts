export type Rng = {
  state: number;
  next: () => number;
  int: (min: number, max: number) => number;
  float: (min: number, max: number) => number;
  chance: (probability: number) => boolean;
  pick: <T>(items: readonly T[]) => T;
};

const MODULUS = 2_147_483_647;
const MULTIPLIER = 48_271;

export const normalizeSeed = (seed: number) => {
  const normalized = Math.abs(Math.floor(seed)) % MODULUS;
  return normalized === 0 ? 1 : normalized;
};

export const createRng = (seed: number): Rng => {
  const rng: Rng = {
    state: normalizeSeed(seed),
    next: () => {
      rng.state = (rng.state * MULTIPLIER) % MODULUS;
      return rng.state / MODULUS;
    },
    int: (min, max) => Math.floor(rng.float(min, max + 1)),
    float: (min, max) => min + (max - min) * rng.next(),
    chance: (probability) => rng.next() < probability,
    pick: (items) => items[Math.floor(rng.next() * items.length)],
  };

  return rng;
};

export const clamp = (value: number, min: number, max: number) => Math.min(max, Math.max(min, value));

export const formatMoney = (value: number) =>
  new Intl.NumberFormat("en-US", {
    style: "currency",
    currency: "USD",
    maximumFractionDigits: value < 100 ? 2 : 0,
  }).format(value);
