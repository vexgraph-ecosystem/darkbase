# darkbase — Repo-Local Living Preferences
> Repo-local preferences governed by the Living Documentation Law.
> Universal Supreme Constitution: preferences.md (vexspoke).

## 0. Constitution Link (supreme)
- [preferences.md](https://github.com/vexgraph-dev/vexspoke/blob/main/preferences.md) (canonical, vexspoke) — accessible locally at ../../preferences.md
- All universal laws in `../../../preferences.md` are mandatory and binding across the ecosystem.
- This document codifies **exclusive** preferences that apply uniquely to `darkbase` (R3 Database Driver).

## 1. Repo-Local Law Index (Binding Matrix)

Universal laws are inherited from the canonical `../../../preferences.md` Index; this table indexes the additional laws specific to this repository.

| Law Title | Scope | Enforcement |
| :--- | :--- | :--- |
| **Zero-Alloc Relational Table Scan Law** | R3 Database Driver | Mandatory for `darkbase` |
| **B-Tree Page Consistency Law** | R3 Database Driver | Mandatory for `darkbase` |

## 2. Exclusive Repo-Local Laws (FULL PROSE RESTATEMENT)

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

;;INTENTION("R3 Database Driver: native vex relational storage substrate; zero-alloc table scans; strict page consistency.")

---

## 4. Readiness Cross-Reference (Living Documentation Law)

- Feature readiness matrix tracked in [`../../_repositories/.ecosystem/darkbase.md`](../../_repositories/.ecosystem/darkbase.md) (rendered as `[[darkbase]]` wiki page).
