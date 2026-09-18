# Reversi AI

A command-line implementation of Reversi (Othello) in C, featuring two different computer opponent strategies: a simple greedy bot and a heuristic-based bot that evaluates board position, not just immediate captures.

## What it does

The game supports an N×N board (up to 26×26) and lets you play as one colour against the computer, which plays the other. Moves are entered as two letters corresponding to row and column (e.g. `de`), matching the labeled board printed each turn.

## The two bots

**`reversi.c` — Greedy bot**
Always plays whichever legal move captures the most opponent tiles that turn. Simple and fast, but shortsighted — it has no concept of long-term board position.

**`reversi_ai.c` — Heuristic bot**
Scores every legal move using a weighted combination of:
- **Capture count** — tiles flipped this turn
- **Corner control** (+100) — corners can never be flipped, making them the most valuable squares on the board
- **Danger-square avoidance** (−50) — squares diagonally adjacent to a corner often hand the opponent access to that corner, so they're penalized
- **Edge preference** (+10) — non-corner edge squares are harder for the opponent to flip
- **Opponent mobility reduction** (×−3) — moves that leave the opponent with fewer legal responses are favored

The bot then plays whichever move scores highest across all five factors combined. This produces noticeably more strategic play than the greedy bot — favoring board control over short-term tile count.

## How to compile and run

Each file is a standalone, complete program.

```bash
# Greedy bot
gcc reversi.c -o reversi
./reversi

# Heuristic bot
gcc reversi_ai.c -o reversi_ai
./reversi_ai
```

You'll be prompted for a board dimension (8 is standard) and which colour the computer plays (B or W). Enter your own moves as two letters, e.g. `de`.

## Credits

`reversi.h` and `reversi_ai.h` were provided as fixed starter interfaces by the APS105 teaching team at the University of Toronto and are unmodified. All game logic, the board representation, and both bot strategies (including the heuristic evaluation function) were designed and implemented independently.

## What I learned

Building the heuristic bot was my first hands-on experience designing a multi-factor evaluation function — balancing several competing scoring terms against each other rather than optimizing for a single metric. It also reinforced how much strategic depth in a simple-looking game like Reversi comes from positional awareness (corners, edges, mobility) rather than raw capture count alone.