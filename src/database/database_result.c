// database/database_result.c — the DatabaseResult row cursor.

#include "database/database_result.h"

#include <stdio.h>

#include "annotation/definition.h"
#include "annotation/overview.h"
#include "darkbase/type.h"
#include "exception/throw.h"
#include "nio/mem.h"
#include "oop/type.h"

;;DEFINITION
/**
 * ============================================================================
 * DEFINITION: DatabaseResult
 * ============================================================================
 * DatabaseResult is the dest-last cursor over one entity's live rows. It
 * borrows the entity's row-pointer ChunkedList (owned by Database) and holds a
 * count snapshot plus an integer cursor, so stepping never allocates and never
 * copies a row: next() reads the stable row slot and writes the bound pointer
 * into the caller's destination. Rows are the caller's own live struct
 * instances; this class never owns or frees them. Cold path only (a query
 * cursor is a cold rendezvous, the Cold-Only Reflection Law). Lifetime: the
 * result is arena-allocated and must not outlive its Database or race a
 * concurrent mutation of the borrowed list.
 * ============================================================================
 */

;;OVERVIEW
/**
 * ============================================================================
 * CLASS: DatabaseResult (database/database_result.c)
 * ============================================================================
 * the DatabaseResult row cursor.
 *
 * STRUCT FIELDS (Mirroring database/database_result.h):
 * ----------------------------------------------------------------------------
 *   DatabaseResult {
 *     ChunkedList *rows; // borrowed row-pointer source
 *     uint32_t count;    // published rows snapshot
 *     uint32_t cursor;   // next row index
 *   }
 *
 * PRIVATE HELPERS (kept file-local, pure logic, no behavior of their own): none.
 *
 * FUNCTION REGISTRY:
 * ----------------------------------------------------------------------------
 * Public Constructors: (.h) DatabaseResult_0(), _1(source)
 * Public Core Functions: (.h) _kind, _check, _next, _count, _position, _rewind
 * Public String Projections: (.h) _toString / _toStringStruct
 * ============================================================================
 */

// CONSTRUCTORS

DatabaseResult *DatabaseResult_0(void) {
    DatabaseResult *self = (DatabaseResult*) Memory_alloc(TYPE_DB_DATABASE_RESULT_SINGLETON, sizeof(DatabaseResult));
    if (self == nullptr)
        return nullptr;
    (*self).rows = nullptr;
    (*self).count = 0u;
    (*self).cursor = 0u;
    return self;
}

DatabaseResult *DatabaseResult_1(ChunkedList *source) {
    if (source == nullptr) {
        THROW("DatabaseResult_1: null row source");
        return nullptr;
    }
    DatabaseResult *self = DatabaseResult_0();
    if (self == nullptr)
        return nullptr;
    (*self).rows = source;
    (*self).count = ChunkedList_size(source);
    return self;
}

void DatabaseResult_free(DatabaseResult *self) {
    if (self != nullptr)
        Memory_free(self);
}

// CORE FUNCTIONS

uint64_t DatabaseResult_kind(const DatabaseResult *self) {
    if (self == nullptr)
        return 0u;
    return Memory_type((void*) self);
}

bool DatabaseResult_check(const DatabaseResult *self, uint64_t typeId) {
    if (self == nullptr)
        return false;
    return Memory_type((void*) self) == typeId;
}

bool DatabaseResult_next(DatabaseResult *self, void **outRow) {
    if (self == nullptr || outRow == nullptr || (*self).rows == nullptr)
        return false;
    uint32_t index = (*self).cursor;
    if (index >= (*self).count)
        return false;
    uint8_t *slot = ChunkedList_slot((*self).rows, index);
    if (slot == nullptr)
        return false;
    (*self).cursor = index + 1u;
    *outRow = *(void**) slot;
    return true;
}

uint32_t DatabaseResult_count(const DatabaseResult *self) {
    return self ? (*self).count : 0u;
}

uint32_t DatabaseResult_position(const DatabaseResult *self) {
    return self ? (*self).cursor : 0u;
}

void DatabaseResult_rewind(DatabaseResult *self) {
    if (self != nullptr)
        (*self).cursor = 0u;
}

// STRING PROJECTIONS (the toString Law)

void DatabaseResult_toString(const DatabaseResult *self, char *dest, size_t cap, bool *outTruncated) {
    if (outTruncated)
        *outTruncated = false;
    if (dest == nullptr || cap == 0u)
        return;
    if (self == nullptr) {
        snprintf(dest, cap, "nullptr");
        return;
    }
    int written = snprintf(dest, cap, "DatabaseResult(%u/%u)", (*self).cursor, (*self).count);
    if (written < 0 || (size_t) written >= cap) {
        if (outTruncated)
            *outTruncated = true;
    }
}

void DatabaseResult_toStringStruct(const DatabaseResult *self, char *dest, size_t cap, bool *outTruncated) {
    if (outTruncated)
        *outTruncated = false;
    if (dest == nullptr || cap == 0u)
        return;
    if (self == nullptr) {
        snprintf(dest, cap, "nullptr");
        return;
    }
    int written = snprintf(dest, cap, "DatabaseResult { rows=0x%llx, count=%u, cursor=%u }",
                           (unsigned long long) (uintptr_t) (*self).rows, (*self).count, (*self).cursor);
    if (written < 0 || (size_t) written >= cap) {
        if (outTruncated)
            *outTruncated = true;
    }
}
