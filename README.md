# darkbase — R3 native vex database store

## CLion: CMake is IDE metadata only

Open this repository root as a CMake project. `CMakeLists.txt` is an IDE-only
blueprint entry: there are no production sources or C23 source targets yet,
so there is nothing to provide semantic diagnostics or inlay hints for.
No fake declarations, dependency downloads, linking or application runner are
wired into it. IDE appearance is user-verified.

Future builds belong to [b](https://github.com/vex-graph/b). No runnable database
target or standalone runtime build is claimed by this metadata entry.

**Role:** R3 Driver — the `Database` interface owner and its native vex store.
**Status:** reserved placeholder (LICENSE only; no store code yet).

## What it is
`darkbase` is where ecosystem persistence lives: the `Database` interface plus
a native vex-backed implementation engineered for bounded, in-budget operation
(no vendor SDKs in the hot path). `api-haven` holds only connector/catalog
shapes (`DbProvider`, `DbSqliteFile` descriptors) — execution delegates here,
per the Vertical Integration Law.

## Depends on (Vertical Integration Law allowlist)
Either R2 public contract: Vexspoke CPU computation/behavior or Relational
Engine memory/storage, stable rows, bindings and native C search over Rust-owned
spans (`+ graphvex` for GPU-backed stores). Database semantics and persistence
remain Darkbase R3. Never R1/R4/R5 or `api-haven` headers. This blueprint has no
implemented engine integration. Migration is staged; existing Vexspoke
memory/container ABI and default allocator remain. R1 owns lifetimes/residency;
no C/Rust atomic-layout compatibility or automatic schema migration is assumed.
GPU shaders/dispatch stay Graphvex R3.

## Layout
- Store (future): `src/` — `Database` interface, native vex backend.
- Tests: the shared `../../../tests` repo will host a `tests/darkbase/` partition
  (mirrored per unit, the Test Tree Mirror Law); no test file lives inside this
  repo's source directories (the Test Segregation Law).

## Laws that govern work here
- Constitution: the [canonical preferences.md Gist](https://gist.github.com/vex-graph/4132a6c45cb6d3797c3e8eff2e94035a); one real, Git-ignored workspace-root `../../../preferences.md`, not a Vexspoke file or symlink.
- Commits land in THIS repo root, one cohesive unit each; never push unless asked.
- One public class per `.h`/`.c` pair, `(*ptr).field` (never `->`), dest-last
  params, `-Wall -Wextra -Werror`.
