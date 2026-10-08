# darkbase — Repo-Local Living Preferences
> Repo-local preferences governed by the Living Documentation Law.
> Universal Supreme Constitution: workspace-root preferences.md, published on Gist.

## 0. Constitution Link (supreme)
- [preferences.md](https://gist.github.com/vex-graph/4132a6c45cb6d3797c3e8eff2e94035a) — real, Git-ignored workspace-root file at ../../../preferences.md, not a tracked Vexspoke file or symlink.
- All universal laws in `../../../preferences.md` are mandatory and binding across the ecosystem.
- This document codifies **exclusive** preferences for `darkbase` (R3 Database Driver). Database semantics and persistence remain R3; allocation/storage and native span search belong to Relational Engine R2, with Vexspoke supplying CPU computation/behavior. Either R2 public contract may be borrowed. This blueprint has no implemented engine integration; migration and durability require owner proof.
- darkbase is a **native vex store, not a SQL engine**. The schema is C reflection; the native query contract is structured, and any text/SQL parser is an optional `cli/` convenience or a foreign driver's private translation.

## 1. Repo-Local Law Index (Binding Matrix)

Universal laws are inherited from the canonical `../../../preferences.md` Index; this table indexes the additional laws specific to this repository.

| Law Title | Scope | Enforcement |
| :--- | :--- | :--- |
| **Entity Model Law** | R3 Database Driver | Mandatory for `darkbase` |
| **Reactive Program Law** | R3 Database Driver | Mandatory for `darkbase` |
| **Store Versus Executable Separation Law** | R3 Database Driver | Mandatory for `darkbase` |
| **Zero-Alloc Relational Table Scan Law** | R3 Database Driver | Mandatory for `darkbase` |
| **B-Tree Page Consistency Law** | R3 Database Driver | Mandatory for `darkbase` |

## 2. Exclusive Repo-Local Laws (FULL PROSE RESTATEMENT)

### Entity Model Law

#### Definition:
darkbase models C vocabulary directly as store vocabulary: a `struct` is an
`Entity` (a table, an ordered list of `EntityField`), a `field` is an
`EntityField` (a physical column: name, byte offset, size, value typeId, and
flags), and a `function` is an `EntityFunction` (a named callable binding).
Plain values are named typed bindings in the same scope. `EntityField` is the
physical descriptor R2 reflection does not provide (its `Field` is
behavior-only), and it is the single source of truth for both persistence and
export.

#### The Why:
A schema expressed as C types needs no DDL text and cannot drift from the struct
it describes. One uniform binding vocabulary lets rows, callable behavior, and
plain values live in one findable namespace, matching the relational engine's
"name to value" thesis.

#### The Rule:
1. **Structs become Entities; fields become EntityFields; functions become EntityFunctions.** No parallel ad-hoc schema language.
2. **EntityField owns the physical layout.** Persistence and export read offsets, size, and typeId from it, never from a hand-maintained table.
3. **One binding namespace.** Entities, functions, and plain values resolve through the same scope and naming grammar.

---

### Reactive Program Law

#### Definition:
darkbase has a reactive layer — the SQL analog of triggers, views, and stored
procedures — as first-class named programs bound to store events
(`beforeInsert`, `afterInsert`, `beforeUpdate`, `afterDelete`, `onOpen`,
`onCommit`, `onRollback`).

#### The Why:
Persistence hooks (validation, indexing, derived views, audit, cache
invalidation) belong to the store, not scattered across every caller. Making
them named programs keeps behavior with the data and exportable with it.

#### The Rule:
1. **Programs fire on the cold admission path only.** Never per element on a hot read (Cold-Strict, Hot-Minimal Validation Law; Cold-Only Reflection Law).
2. **Programs are data plus behavior.** They export as a descriptor; their code rebinds by name on load, never by a stored address.
3. **Views are derived Entities.** A view's rows are computed by a program against the authoritative store, not a second synchronized copy.

---

### Store Versus Executable Separation Law

#### Definition:
The R3 repository is the `Database` interface and native vex store (future name
`darkbase-db`). The R5 executable is `darkbase`, an interactive host that opens a
`.vexdb` and works with it.

#### The Why:
The store must remain a resident library that any host borrows; the workbench is
an application with its own lifecycle. Conflating them couples the store to a
UI and blocks other consumers.

#### The Rule:
1. **The store never includes R5.** Hosts borrow an opaque `Database*` plus callbacks (Vertical Integration Law).
2. **The executable is a separate application** over the store, owning no store internals.
3. **Naming is deliberate:** store = `darkbase-db` (future), executable = `darkbase`.

---

### Zero-Alloc Relational Table Scan Law

#### Definition:
Table scans, range queries, and projection operations in `darkbase` operate directly across memory-mapped relational row slices using strided record offsets with zero heap allocation during query execution.

#### The Why:
Database query engines that allocate row objects during iteration thrash caches and bottleneck on the memory allocator. Strided memory-mapped scanning enables near-memory-bus throughput.

#### The Rule:
1. **Strided Projections:** Query iterators yield pointers to existing record spans.
2. **Zero Row Allocs:** Never allocate temporary row wrappers in scan loops.

---

### B-Tree Page Consistency Law

#### Definition:
B-tree node splits, leaf balance operations, and index updates execute strictly within fixed-size page blocks allocated from dedicated database memory pools.

#### The Why:
Predictable index geometry guarantees bounded traversal depth and prevents fragmentation across database storage files.

#### The Rule:
1. **Page Bounded:** B-tree pages are fixed-size power-of-two blocks.
2. **Atomic Splits:** Page balance updates commit transactionally before updating parent pointers.

---

## 3. Repo-Local Extensions (managed, per the Conflict Triage Law)

;;INTENTION("R3 Database Driver: native vex entity store (struct->Entity, field->EntityField, function->EntityFunction) with reactive store-event programs; zero-alloc table scans; strict page consistency; structured native query, not a SQL engine.")

---

## 4. Readiness Cross-Reference (Living Documentation Law)

- Feature readiness matrix: [darkbase](../../ecosystem/darkbase.md), rendered as `[[darkbase]]`.
