#!/usr/bin/env python3
"""Visual-ACK blink simulation for DMX_45CH app_CmdBlink + app_CMD.

Models: 1 s non-blocking RGB override, per-command colors, DISCOVER slot
wait (blocking HAL_Delay), SELECT match-only blink, IWDG 410 ms budget.

Usage:
    python3 blink-simulation.py            # ASCII timeline + write SVG
    python3 blink-simulation.py --animate  # live terminal replay (ANSI)
    python3 blink-simulation.py --svg out.svg
"""
import sys
import time

BLINK_MS = 1000
IWDG_MS = 410
COLORS = {
    "GET_INFO": (0, 255, 255),
    "DISCOVER": (255, 0, 255),
    "SELECT_MATCH": (0, 255, 0),
    "DESELECT": (0, 0, 255),
}
# (time_ms, event, extra): DISCOVER extra = slot wait ms before TX
EVENTS = [
    (0, "GET_INFO", 0),
    (1500, "DISCOVER", 35),
    (3000, "SELECT match", 0),
    (3600, "GET_INFO", 0),      # overlapping: retriggers over green
    (4200, "SELECT mismatch", 0),
    (5200, "DESELECT", 0),
]
END_MS = 6600
DMX = None


def simulate(step_ms=10):
    """Return list of (t_start, t_end, color_or_None, label)."""
    segs, ev_i = [], 0
    active, expiry = DMX, -1
    cur_start, cur_col = 0, DMX
    for t in range(0, END_MS + step_ms, step_ms):
        while ev_i < len(EVENTS) and EVENTS[ev_i][0] <= t:
            _, e, extra = EVENTS[ev_i]
            ev_i += 1
            if e == "DISCOVER":
                active, expiry = COLORS[e], t + extra + BLINK_MS
            elif e == "SELECT mismatch":
                pass
            else:
                key = "SELECT_MATCH" if e.startswith("SELECT") else e
                active, expiry = COLORS[key], t + BLINK_MS
        col = active if (active is not DMX and t < expiry) else DMX
        if col != cur_col:
            segs.append((cur_start, t, cur_col))
            cur_start, cur_col = t, col
    segs.append((cur_start, END_MS, cur_col))
    return segs


def rgb_str(c):
    return "DMX" if c is DMX else "RGB%s" % (c,)


def ascii_timeline():
    print("%7s  LED output (45ch)" % "t(ms)")
    for s, e, c in simulate(100):
        print("%7d  %s" % (s, rgb_str(c)))


def write_svg(path):
    W, H, PAD = 1000, 220, 60
    sx = lambda t: PAD + (W - 2 * PAD) * t / END_MS  # noqa: E731
    y0, lane_h = 60, 60
    parts = [
        '<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d">' % (W, H),
        '<style>text{font-family:monospace;font-size:11px}</style>',
        '<text x="%d" y="24">DMX_45CH visual-ACK blink simulation (1 s, LEDs only)</text>' % PAD,
    ]
    for s, e, c in simulate(10):
        fill = "#222" if c is DMX else "#%02x%02x%02x" % c
        parts.append('<rect x="%.1f" y="%d" width="%.1f" height="%d" fill="%s"/>' % (
            sx(s), y0, max(sx(e) - sx(s), 1), lane_h, fill))
        if c is not DMX:
            parts.append('<text x="%.1f" y="%d" fill="#fff">%s</text>' % (
                sx(s) + 3, y0 + 22, rgb_str(c)))
    # event markers + IWDG note
    for t, e, extra in EVENTS:
        parts.append('<line x1="%.1f" y1="%d" x2="%.1f" y2="%d" stroke="red"/>' % (
            sx(t), y0 + lane_h, sx(t), y0 + lane_h + 30))
        parts.append('<text x="%.1f" y="%d">%s</text>' % (sx(t) + 2, y0 + lane_h + 42, e))
        if extra:
            parts.append('<rect x="%.1f" y="%d" width="%.1f" height="8" fill="orange"/>' % (
                sx(t), y0 + lane_h + 8, sx(extra)))
    parts.append('<text x="%d" y="%d">Longest main-loop block: 75 ms slot wait'
                 ' &lt; IWDG 410 ms. Orange = DISCOVER slot wait. Mismatch: no blink.</text>' % (
                     PAD, y0 + lane_h + 62))
    parts.append("</svg>")
    with open(path, "w") as f:
        f.write("\n".join(parts) + "\n")
    print("Wrote", path)


def animate():
    segs = simulate(50)
    print("Live replay (1 s = 1 s). Ctrl-C to stop.")
    print("Legend: actual RGB background per active blink, '.' = DMX.")
    t0 = time.time()
    try:
        while True:
            t = int((time.time() - t0) * 1000) % (END_MS + 800)
            col = next((c for s, e, c in segs if s <= t < e), DMX)
            if col is DMX:
                bar = "." * 40
            else:
                bar = "\x1b[48;2;%d;%d;%dm%s\x1b[0m" % (col + (" " * 40,))
            sys.stdout.write("\rt=%4dms %s %s" % (t, bar, rgb_str(col)))
            sys.stdout.flush()
            time.sleep(0.05)
    except KeyboardInterrupt:
        print()


if __name__ == "__main__":
    if "--animate" in sys.argv:
        animate()
    else:
        out = sys.argv[sys.argv.index("--svg") + 1] \
            if "--svg" in sys.argv else "blink-simulation.svg"
        ascii_timeline()
        write_svg(out)
