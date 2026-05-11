import type { Crisis } from "../game/types";

type CrisisPanelProps = {
  crises: Crisis[];
  onResolve: (crisisId: string, choiceId: string) => void;
};

export function CrisisPanel({ crises, onResolve }: CrisisPanelProps) {
  return (
    <section className="panel p-4">
      <h2 className="section-title">crisis queue</h2>
      <div className="mt-4 space-y-3">
        {crises.length === 0 ? (
          <p className="rounded border border-slate-700/60 bg-black/20 p-4 text-sm text-slate-500">
            No active disaster. This is statistically suspicious.
          </p>
        ) : (
          crises.map((crisis) => {
            const isMutation = crisis.mutationGeneration !== undefined && crisis.mutationGeneration > 0;
            return (
              <article
                className={`rounded border p-4 shadow-[0_0_30px_rgba(248,113,113,0.08)] ${
                  isMutation ? "border-purple-400/30 bg-purple-950/20" : "border-red-400/30 bg-red-950/20"
                }`}
                key={crisis.id}
              >
                <div className="flex items-start justify-between gap-3">
                  <div>
                    <h3 className={`font-mono text-base uppercase ${isMutation ? "text-purple-100" : "text-red-100"}`}>
                      {isMutation && <span className="mr-1.5 text-purple-400">◆</span>}
                      {crisis.title}
                    </h3>
                    <p className="mt-1 text-sm leading-6 text-slate-300">{crisis.body}</p>
                  </div>
                  <div className="flex flex-col items-end gap-1">
                    <span className={`rounded px-2 py-1 font-mono text-xs ${isMutation ? "bg-purple-400/10 text-purple-200" : "bg-red-400/10 text-red-200"}`}>
                      severity {Math.round(crisis.severity)}
                    </span>
                    {isMutation && (
                      <span className="font-mono text-[10px] text-purple-300">
                        gen {crisis.mutationGeneration}
                      </span>
                    )}
                  </div>
                </div>
                <div className={`mt-3 font-mono text-xs uppercase tracking-[0.18em] ${isMutation ? "text-purple-200/70" : "text-red-200/70"}`}>
                  {isMutation ? "mutates again" : "mutates"} at T+{crisis.expiresAt} if ignored
                </div>
                <div className="mt-4 grid gap-2">
                  {crisis.choices.map((choice) => (
                    <button className="button-secondary text-left" key={choice.id} type="button" onClick={() => onResolve(crisis.id, choice.id)}>
                      <span className="block text-sm text-slate-100">{choice.label}</span>
                      <span className="mt-1 block text-xs normal-case tracking-normal text-slate-500">{choice.description}</span>
                    </button>
                  ))}
                </div>
              </article>
            );
          })
        )}
      </div>
    </section>
  );
}
