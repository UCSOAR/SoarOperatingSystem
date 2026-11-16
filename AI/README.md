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

## Using it in a board repo

From the board repo root (use `python` instead of `python3` on Windows):

```sh
cp SoarOS/AI/BOARD.template.md BOARD.md   # once, then fill it in
python3 SoarOS/AI/sync.py                 # generate / update the files below
python3 SoarOS/AI/sync.py --check         # CI: exit 1 if anything is out of date
```

Commit the generated files. Rerun the script whenever you bump SoarOS or edit
`BOARD.md`. It needs only Python 3.8+ and the standard library.

`AI/sync.py` generates:

| Generated file           | Read by        | Contents                    |
| ------------------------ | -------------- | --------------------------- |
| `AGENTS.md`              | Codex, Gemini  | `AI/AGENTS.md` + `BOARD.md` |
| `CLAUDE.md`, `GEMINI.md` | Claude, Gemini | `@AGENTS.md`                |
| `.claude/skills/`        | Claude Code    | copy of `AI/skills/`        |
| `.agents/skills/`        | Codex          | copy of `AI/skills/`        |

The generated files are real copies (no symlinks), so they work on Windows, macOS
and Linux. Details:

- Only skill folders containing a `SKILL.md` are synced, so placeholders are
  skipped. Synced skill folders get a `.soar-ai-generated` marker, and skills a
  board adds itself are never touched.
- Skills or files removed from `SoarOS/AI/skills/` are removed from the board on
  the next sync.
- The script refuses to overwrite a hand-written `AGENTS.md`, `CLAUDE.md` or
  `GEMINI.md`. Move board-specific content into `BOARD.md`, or pass `--force`.
- Line endings are normalized, so Windows `autocrlf` checkouts don't show up as
  out of date.

### Pre-commit hook (optional)

Add this to the board's `.pre-commit-config.yaml` to regenerate automatically.
`language: python` makes pre-commit supply the interpreter, so it works on every
OS:

```yaml
- repo: local
  hooks:
    - id: soar-ai-sync
      name: Sync SOAR AI files
      entry: python SoarOS/AI/sync.py
      language: python
      pass_filenames: false
      always_run: true
```

## Rules for this folder

- **Never add `.c`, `.cpp`, `.h` or `.hpp` files here.** SoarOS is compiled into
  every board's firmware by CubeIDE. Put code examples inside Markdown.
- Exclude `SoarOS/AI/` from the build in each CubeIDE project as a safeguard.
- Keep `AGENTS.md` well under 32 KiB, Codex's default instruction size limit.
  Detailed material belongs in skill reference files.
- Rules marked **(proposed)** in `AGENTS.md` need confirmation from the software
  leads before they are treated as policy.
