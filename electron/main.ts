import Database from "better-sqlite3";
import { app, BrowserWindow, ipcMain } from "electron";
import { dirname, join } from "node:path";
import { fileURLToPath } from "node:url";

const __dirname = dirname(fileURLToPath(import.meta.url));

type SavedRun = {
  id: string;
  savedAt: string;
  payload: string;
};

class SaveRepository {
  private readonly db: Database.Database;

  constructor(dbPath: string) {
    this.db = new Database(dbPath);
    this.db.pragma("journal_mode = WAL");
    this.db.exec(`
      CREATE TABLE IF NOT EXISTS runs (
        id TEXT PRIMARY KEY,
        saved_at TEXT NOT NULL,
        payload TEXT NOT NULL
      );
    `);
  }

  saveRun(id: string, payload: unknown): SavedRun {
    const savedAt = new Date().toISOString();
    const serialized = JSON.stringify(payload);

    this.db
      .prepare(
        "INSERT INTO runs (id, saved_at, payload) VALUES (?, ?, ?) ON CONFLICT(id) DO UPDATE SET saved_at = excluded.saved_at, payload = excluded.payload",
      )
      .run(id, savedAt, serialized);

    return { id, savedAt, payload: serialized };
  }

  latestRun(): SavedRun | null {
    return (
      this.db.prepare("SELECT id, saved_at as savedAt, payload FROM runs ORDER BY saved_at DESC LIMIT 1").get() as
        | SavedRun
        | undefined
    ) ?? null;
  }
}

let mainWindow: BrowserWindow | null = null;
let saves: SaveRepository | null = null;

const createWindow = async () => {
  mainWindow = new BrowserWindow({
    width: 1360,
    height: 920,
    minWidth: 1180,
    minHeight: 760,
    title: "Pump.exe",
    backgroundColor: "#05070a",
    webPreferences: {
      preload: join(__dirname, "preload.js"),
      contextIsolation: true,
      nodeIntegration: false,
    },
  });

  if (process.env.ELECTRON_RENDERER_URL) {
    await mainWindow.loadURL(process.env.ELECTRON_RENDERER_URL);
    mainWindow.webContents.openDevTools({ mode: "detach" });
    return;
  }

  await mainWindow.loadFile(join(__dirname, "../../dist/index.html"));
};

app.whenReady().then(async () => {
  saves = new SaveRepository(join(app.getPath("userData"), "pump-exe.sqlite"));

  ipcMain.handle("save:run", (_event, id: string, payload: unknown) => saves?.saveRun(id, payload));
  ipcMain.handle("load:latest-run", () => saves?.latestRun());

  await createWindow();

  app.on("activate", () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      void createWindow();
    }
  });
});

app.on("window-all-closed", () => {
  if (process.platform !== "darwin") {
    app.quit();
  }
});
