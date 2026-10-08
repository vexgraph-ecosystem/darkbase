#ifndef DARKBASE_DATABASE_DATABASE_RESULT_H
#define DARKBASE_DATABASE_DATABASE_RESULT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "c23/constructor.h"
#include "struct/chunked_list.h"

// database/database_result.h — the DatabaseResult row cursor.
//
// A DatabaseResult borrows ONE entity's row-pointer list (the ChunkedList a
// Database owns; it is never freed while the Database lives) and walks it with
// an index cursor. It copies nothing: next() writes the bound row pointer into
// a caller destination (dest-last, the Semantic Consistency Law (Argument
// order)), so a query is zero-copy. Cold path only. A result must not outlive
// its Database, and a caller must not mutate the source list concurrently.

typedef struct DatabaseResult {
    ChunkedList *rows; // borrowed row-pointer source (never freed here)
    uint32_t count;    // published rows snapshotted at construction
    uint32_t cursor;   // index of the next row to hand out
} DatabaseResult;

// --- Constructors ---
// _0() is the empty cursor (no source, exhausted). _1(source) borrows the row
// list; null source returns nullptr (cold rejection). Arena-allocated.
DatabaseResult *DatabaseResult_0(void);
DatabaseResult *DatabaseResult_1(ChunkedList *source);
#define DatabaseResult(...) CONSTRUCTOR_DISPATCH(DatabaseResult, __VA_ARGS__)

// Release an arena-allocated result (a constructor result). The borrowed row
// list is never freed.
void DatabaseResult_free(DatabaseResult *self);

uint64_t DatabaseResult_kind(const DatabaseResult *self);
bool DatabaseResult_check(const DatabaseResult *self, uint64_t typeId);

// --- Core functions ---
// Advance and write the next row pointer into outRow (dest-last). False at the
// end of the result or on a null argument; outRow is untouched on false.
bool DatabaseResult_next(DatabaseResult *self, void **outRow);
// Rows snapshotted at construction.
uint32_t DatabaseResult_count(const DatabaseResult *self);
// Index of the next row (== consumed rows).
uint32_t DatabaseResult_position(const DatabaseResult *self);
// Reset the cursor to the first row.
void DatabaseResult_rewind(DatabaseResult *self);

void DatabaseResult_toString(const DatabaseResult *self, char *dest, size_t cap, bool *outTruncated);
void DatabaseResult_toStringStruct(const DatabaseResult *self, char *dest, size_t cap, bool *outTruncated);

#endif
