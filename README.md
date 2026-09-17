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
- Tests: umbrella `tests/` has no `darkbase/` partition yet; until then keep
  seam tests in-repo under `tests/` (never inside source dirs, per the Test
  Segregation Law).

## Laws that govern work here
- Constitution: `../../preferences.md` (umbrella symlink → `ecosystem/vexspoke/preferences.md`).
- Commits land in THIS repo root, one cohesive unit each; never push unless asked.
- One public class per `.h`/`.c` pair, `(*ptr).field` (never `->`), dest-last
  params, `-Wall -Wextra -Werror`.
