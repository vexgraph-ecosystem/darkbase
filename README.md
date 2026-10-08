# darkbase — R3 native vex database store

## CLion: CMake is IDE metadata only

Open this repository root as a CMake project. `CMakeLists.txt` is an IDE-only
blueprint entry: the registry header `src/darkbase/type.h` exists, but there are
no production sources or C23 source targets yet, so there is nothing to provide
semantic diagnostics or inlay hints for. No fake declarations, dependency
downloads, linking or application runner are wired into it. IDE appearance is
user-verified.

Future builds belong to [b](https://github.com/vex-graph/b). No runnable database
target is claimed by this metadata entry.

**Role:** R3 Driver — the `Database` interface owner and its native vex store.
**Status:** foundations (registry + build wiring). No store behavior yet.

## What it is

`darkbase` is where ecosystem persistence lives: the `Database` interface plus a
native vex-backed implementation engineered for bounded, in-budget operation (no
vendor SDKs in the hot path). `api-haven` holds only connector/catalog shapes
(`DbProvider`, `DbSqliteFile` descriptors) — execution delegates here, per the
Vertical Integration Law.

It is **not** a SQL engine. The native contract is a structured relational API;
the schema is C reflection, not DDL. Text/SQL appears only as an optional
`cli/` convenience or a foreign driver's private translation, never in the core.

## The entity model

| C vocabulary | darkbase class | Meaning |
| :--- | :--- | :--- |
| `struct` | `Entity` | a table: an ordered `EntityField` list |
| `field` | `EntityField` | a physical column: name, offset, size, value typeId, flags |
| `function` | `EntityFunction` | a named callable binding |
| `trigger` | `DbTrigger` | a reactive program bound to a store event |

The physical descriptor `EntityField` is the keystone: R2 reflection's `Field`
is behavior-only (name + read/set + target) and carries no offset/size/typeId, so
darkbase owns the descriptor both persistence and export depend on.

Plain values are named typed bindings in the same scope, so structs, functions,
and values share one reflection vocabulary.

## Reactive programs

Named programs bound to store events are the SQL analog of triggers, views, and
stored procedures. A program declares an event (`beforeInsert`, `afterInsert`,
`beforeUpdate`, `afterDelete`, `onOpen`, `onCommit`, `onRollback`) plus a
callback, and the store fires it on the cold admission path — never per element
on a hot read (Cold-Strict, Hot-Minimal Validation Law; Cold-Only Reflection
Law). Views are derived `Entity` rows computed by a program. Programs export as
a descriptor; their code rebinds by name on load, like functions.

## Future naming: store vs executable

- R3 (this repository) is the `Database` interface and native vex store. A future
  rename makes it `darkbase-db`.
- R5 (future) is `darkbase`: the interactive executable that opens a `.vexdb` and
  works with it — an application over the R3 library via opaque handles, never an
  include. `darkbase-interactable` is that host.

## Depends on (Vertical Integration Law allowlist)

Either R2 public contract: Vexspoke CPU computation/behavior (`oop/type.h`,
`reflection/*`) or Relational Engine memory/storage (`nio/mem.h`, `io/file.h`,
`io/vexhome.h`, stable rows, `VariableRegistry`, native C search). Never
R1/R4/R5 or `api-haven` headers. GPU shaders, dispatch and graphics composition
stay Graphvex R3; a GPU-backed store is a future borrow, not a dependency.
Database semantics and persistence remain Darkbase R3.

## Layout

- `src/darkbase/type.h` — the class registry (`PROJ_DARKBASE`, 1..N).
- `src/database/` — the L2 `Database` interface and cursor (future).
- `src/schema/` — `Entity`, `EntityField`, `EntityFunction` (future).
- `src/store/`, `src/codec/`, `src/index/`, `src/tx/`, `src/export/` (future).
- `drivers/` — quarantined external driver dylibs (future: sqlite, postgres, ...).
- Tests: the shared `../../../tests/darkbase/` partition (mirrored per unit, the
  Test Tree Mirror Law); no test file lives inside this repo's source
  directories (the Test Segregation Law).

## Laws that govern work here

- Constitution: the [canonical preferences.md Gist](https://gist.github.com/vex-graph/4132a6c45cb6d3797c3e8eff2e94035a); one real, Git-ignored workspace-root `../../../preferences.md`, not a Vexspoke file or symlink.
- Commits land in THIS repo root, one cohesive unit each; never push unless asked.
- One public class per `.h`/`.c` pair, `(*ptr).field` (never `->`), dest-last
  params, `-Wall -Wextra -Werror`.
