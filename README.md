# darkbase — R3 native vex database store

**Role:** R3 Driver — the `Database` interface owner and its native vex store.
**Status:** reserved placeholder (LICENSE only; no store code yet).

## What it is
`darkbase` is where ecosystem persistence lives: the `Database` interface plus
a native vex-backed implementation engineered for bounded, in-budget operation
(no vendor SDKs in the hot path). `api-haven` holds only connector/catalog
shapes (`DbProvider`, `DbSqliteFile` descriptors) — execution delegates here,
per the Vertical Integration Law.

## Depends on (Vertical Integration Law allowlist)
`vexspoke` (`+ graphvex` for GPU-backed stores) only. Never engines, never
`darling`/`api-haven` headers.

## Layout
- Store (future): `src/` — `Database` interface, native vex backend.
- Tests: the shared `tests/` repo will host a `tests/darkbase/` partition
  (mirrored per unit, the Test Tree Mirror Law); no test file lives inside this
  repo's source directories (the Test Segregation Law).

## Laws that govern work here
- Constitution: the universal [`preferences.md`](../../vexspoke/preferences.md) (canonical file at `ecosystem/vexspoke/preferences.md`; the workspace root links to it).
- Commits land in THIS repo root, one cohesive unit each; never push unless asked.
- One public class per `.h`/`.c` pair, `(*ptr).field` (never `->`), dest-last
  params, `-Wall -Wextra -Werror`.
