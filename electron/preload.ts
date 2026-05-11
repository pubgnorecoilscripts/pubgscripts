import { contextBridge, ipcRenderer } from "electron";

contextBridge.exposeInMainWorld("pumpDesktop", {
  saveRun: (id: string, payload: unknown) => ipcRenderer.invoke("save:run", id, payload),
  loadLatestRun: () => ipcRenderer.invoke("load:latest-run"),
});
