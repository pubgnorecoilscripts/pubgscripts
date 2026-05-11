import type { SocialPost } from "../game/types";

type FeedPanelProps = {
  feed: SocialPost[];
  frenzyActive?: boolean;
};

const sentimentStyle = {
  bullish: "border-emerald-400/20 text-emerald-200",
  bearish: "border-red-400/25 text-red-200",
  unhinged: "border-pink-400/25 text-pink-200",
  regulatory: "border-yellow-300/25 text-yellow-200",
  bot: "border-cyan-300/20 text-cyan-200",
};

export function FeedPanel({ feed, frenzyActive }: FeedPanelProps) {
  return (
    <section className={`panel p-4 transition-all ${frenzyActive ? "border-pink-400/30" : ""}`}>
      <div className="flex items-center justify-between">
        <h2 className="section-title">fake social feed</h2>
        <span className={`font-mono text-xs ${frenzyActive ? "text-pink-300 animate-pulse" : "text-pink-300 animate-pulse"}`}>
          {frenzyActive ? "OVERLOADED" : "LIVE"}
        </span>
      </div>
      <div className={`mt-4 max-h-[520px] space-y-3 overflow-auto pr-1 ${frenzyActive ? "feed-frenzy" : ""}`}>
        {feed.map((post) => (
          <article className={`rounded border bg-black/20 p-3 ${sentimentStyle[post.sentiment]}`} key={post.id}>
            <div className="flex items-center justify-between gap-2 font-mono text-xs">
              <span>{post.author}</span>
              <span className="text-slate-600">T+{post.tick}</span>
            </div>
            <p className="mt-2 text-sm leading-6 text-slate-200">{post.body}</p>
            <div className="mt-2 flex items-center gap-3 font-mono text-[10px] uppercase tracking-[0.16em] text-slate-500">
              <span>
                impact {post.impact >= 0 ? "+" : ""}
                {Math.round(post.impact)}
              </span>
              {post.factionSource && <span className="text-purple-300">{post.factionSource}</span>}
            </div>
          </article>
        ))}
      </div>
    </section>
  );
}
