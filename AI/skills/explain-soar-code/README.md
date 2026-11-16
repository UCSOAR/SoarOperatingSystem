# explain-soar-code (placeholder)

Planned skill: explains SOAR firmware to a developer. It traces how a task is
initialized in `run_main()`, how `Command`s flow between task queues and through
`DataBroker`, and how ISRs hand data to tasks, using the board's own code.

Not written yet. When it is, this folder will contain a `SKILL.md`, the shared
format read by Claude Code, Codex and Gemini CLI. Keep any code examples inside
`.md` files: SoarOS is compiled by CubeIDE, so a `.c` or `.cpp` file here would be
built into firmware.
