import type { SocialPost } from "../game/types";

type FeedPanelProps = {
  feed: SocialPost[];
};

const sentimentStyle = {
  bullish: "border-emerald-400/20 text-emerald-200",
  bearish: "border-red-400/25 text-red-200",
  unhinged: "border-pink-400/25 text-pink-200",
  regulatory: "border-yellow-300/25 text-yellow-200",
  bot: "border-cyan-300/20 text-cyan-200",
};

export function FeedPanel({ feed }: FeedPanelProps) {
  return (
    <section className="panel p-4">
      <div className="flex items-center justify-between">
        <h2 className="section-title">fake social feed</h2>
        <span className="animate-pulse font-mono text-xs text-pink-300">LIVE</span>
      </div>
      <div className="mt-4 max-h-[520px] space-y-3 overflow-auto pr-1">
        {feed.map((post) => (
          <article className={`rounded border bg-black/20 p-3 ${sentimentStyle[post.sentiment]}`} key={post.id}>
            <div className="flex items-center justify-between gap-2 font-mono text-xs">
              <span>{post.author}</span>
              <span className="text-slate-600">T+{post.tick}</span>
            </div>
            <p className="mt-2 text-sm leading-6 text-slate-200">{post.body}</p>
          </article>
        ))}
      </div>
    </section>
  );
}
