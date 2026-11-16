# soar-backend-review (placeholder)

Planned skill: reviews a diff against SOAR paradigms and forbidden practices, and
reports violations with file and line, the rule broken, and the fix.

Not written yet. When it is, this folder will contain:

- `SKILL.md`: the review procedure and output format.
- `references/forbidden.md`: banned practices, each with why and the correct
  alternative.
- `references/paradigms.md`: the expected SOAR patterns (tasks, Commands,
  DataBroker, ISRs, memory).

These will build on the rules in `../../AGENTS.md`. Rules that can be checked
mechanically (e.g. `printf`, `HAL_Delay` in tasks, `malloc` / `free`) should also
become plain scripted checks in CI, not only AI review.
