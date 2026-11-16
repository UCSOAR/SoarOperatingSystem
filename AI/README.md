# SoarOS AI

Shared instructions and skills for AI coding agents (Codex, Claude Code, Gemini
CLI) used in SOAR firmware repositories.

These live in SoarOS because every board includes SoarOS at the same path
(`SoarOS/`), and boards are required to track SoarOS `master`. Updating SoarOS
therefore updates the AI rules too.

## Layout

```
AI/
  AGENTS.md            Shared rules for all AI tools (source of truth)
  BOARD.template.md    Template for each board's BOARD.md
  skills/
    explain-soar-code/     (placeholder) explain firmware structure and data flow
    soar-backend-review/   (placeholder) review diffs against SOAR rules
```

## How it will be used (planned, not built yet)

A cross-platform Python script (`AI/sync.py`) will generate these files in each
board repo:

| Generated file                  | Read by        | Contents                     |
| ------------------------------- | -------------- | ---------------------------- |
| `AGENTS.md`                     | Codex, Gemini  | `AI/AGENTS.md` + `BOARD.md`  |
| `CLAUDE.md`, `GEMINI.md`        | Claude, Gemini | `@AGENTS.md`                 |
| `.claude/skills/`               | Claude Code    | copy of `AI/skills/`         |
| `.agents/skills/`               | Codex          | copy of `AI/skills/`         |

The generated files are real copies (no symlinks), committed to the board repo,
so they work on Windows, macOS and Linux. A pre-commit hook will regenerate them,
and CI will fail if they are out of date.

## Rules for this folder

- **Never add `.c`, `.cpp`, `.h` or `.hpp` files here.** SoarOS is compiled into
  every board's firmware by CubeIDE. Put code examples inside Markdown.
- Exclude `SoarOS/AI/` from the build in each CubeIDE project as a safeguard.
- Keep `AGENTS.md` well under 32 KiB, Codex's default instruction size limit.
  Detailed material belongs in skill reference files.
- Rules marked **(proposed)** in `AGENTS.md` need confirmation from the software
  leads before they are treated as policy.
