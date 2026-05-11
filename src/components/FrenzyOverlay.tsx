type FrenzyOverlayProps = {
  intensity: number;
};

export function FrenzyOverlay({ intensity }: FrenzyOverlayProps) {
  const opacity = Math.min(0.18, intensity * 0.07);
  const glowSize = 20 + intensity * 15;

  return (
    <div className="pointer-events-none fixed inset-0 z-40" aria-hidden>
      <div
        className="absolute inset-0"
        style={{
          background: `radial-gradient(ellipse at 50% 30%, rgba(236, 72, 153, ${opacity}) 0%, transparent ${glowSize}%)`,
          animation: "frenzyPulse 0.8s infinite ease-in-out",
        }}
      />
      <div
        className="absolute inset-0"
        style={{
          background: `radial-gradient(ellipse at 80% 70%, rgba(168, 85, 247, ${opacity * 0.6}) 0%, transparent ${glowSize * 0.7}%)`,
          animation: "frenzyPulse 1.2s infinite ease-in-out reverse",
        }}
      />
      {intensity > 1.5 && (
        <div
          className="absolute inset-0"
          style={{
            background: "repeating-linear-gradient(0deg, transparent, transparent 3px, rgba(236, 72, 153, 0.03) 3px, rgba(236, 72, 153, 0.03) 4px)",
            animation: "scanlines 0.1s infinite linear",
          }}
        />
      )}
    </div>
  );
}
