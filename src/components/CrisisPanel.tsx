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
          crises.map((crisis) => (
            <article className="rounded border border-red-400/30 bg-red-950/20 p-4 shadow-[0_0_30px_rgba(248,113,113,0.08)]" key={crisis.id}>
              <div className="flex items-start justify-between gap-3">
                <div>
                  <h3 className="font-mono text-base uppercase text-red-100">{crisis.title}</h3>
                  <p className="mt-1 text-sm leading-6 text-slate-300">{crisis.body}</p>
                </div>
                <span className="rounded bg-red-400/10 px-2 py-1 font-mono text-xs text-red-200">
                  severity {Math.round(crisis.severity)}
                </span>
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
          ))
        )}
      </div>
    </section>
  );
}
