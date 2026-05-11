/// <reference types="vite/client" />

type SavedRun = {
  id: string;
  savedAt: string;
  payload: string;
};

interface Window {
  pumpDesktop?: {
    saveRun: (id: string, payload: unknown) => Promise<SavedRun>;
    loadLatestRun: () => Promise<SavedRun | null>;
  };
}
