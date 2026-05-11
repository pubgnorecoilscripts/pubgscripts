type TerminalPanelProps = {
  lines: string[];
};

export function TerminalPanel({ lines }: TerminalPanelProps) {
  return (
    <section className="panel p-4">
      <h2 className="section-title">system log</h2>
      <div className="mt-4 max-h-72 overflow-auto rounded border border-cyan-400/10 bg-black/50 p-3 font-mono text-xs leading-6 text-cyan-100">
        {lines.map((line) => (
          <div key={line}>{line}</div>
        ))}
      </div>
    </section>
  );
}
