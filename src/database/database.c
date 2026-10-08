// database/database.c — the L2 Database: the entity/row switchboard.

#include "database/database.h"

#include <stdio.h>
#include <string.h>

#include "annotation/definition.h"
#include "annotation/overview.h"
#include "darkbase/type.h"
#include "exception/throw.h"
#include "nio/mem.h"
#include "oop/type.h"
#include "reflection/struct.h"
#include "relational/variable_slot.h"

;;DEFINITION
/**
 * ============================================================================
 * DEFINITION: Database
 * ============================================================================
 * Database is the native vex store's in-memory switchboard and the R3 entry to
 * persistence. It registers reflection Structs as entities (tables) and binds
 * caller-owned live rows to them, storing only stable row POINTERS — it never
 * copies or owns a row. This is the "everything is a pointer" thesis applied to
 * storage: an entity IS a Struct, its columns ARE Fields (behavior + physical
 * layout), a row is the caller's struct instance, and Database is the name ->
 * entity -> rows index.
 *
 * The entity registry is a never-moved ChunkedList of private DbEntity slot
 * records (name, borrowed schema, per-entity row list). Each row list is a
 * ChunkedList of void* with stable addresses, so a row pointer handed to a
 * DatabaseResult stays valid across growth until the Database is freed.
 *
 * Cold rejections (null/illegal name, duplicate, absent) are recorded on the
 * instance and reported once through THROW (the Cold-Strict, Hot-Minimal
 * Validation Law; the THROW Law). The class is owner-affine: one writer at a
 * time; a caller excludes concurrent mutation while a DatabaseResult walks.
 *
 * Lifetime: arena-allocated (a constructor result). Database_free releases the
 * per-entity row lists and the registry, then the block; borrowed rows and
 * schemas are never freed here. Cold path only for the reflective name lookups
 * (the Cold-Only Reflection Law): consumers that need a bind repeatedly resolve
 * once and hold the index/slot.
 * ============================================================================
 */

;;OVERVIEW
/**
 * ============================================================================
 * CLASS: Database (database/database.c)
 * ============================================================================
 * the L2 Database entity/row switchboard.
 *
 * STRUCT FIELDS (Mirroring database/database.h):
 * ----------------------------------------------------------------------------
 *   Database {
 *     ChunkedList *entities;           // DbEntity rows
 *     uint32_t chunkBytes;             // leaf budget for per-entity row lists
 *     int32_t lastCode;                // last DATABASE_* code
 *     char lastError[DB_ERROR_MESSAGE_MAX]; // last rejection text
 *   }
 *
 * PRIVATE HELPERS (kept file-local, pure logic):
 *   findEntity(self, folded)   // linear name lookup over the entity rows (static)
 *   recordError(self, code, message) // store the last result + text (static)
 *
 * SLOT RECORD (owned by Database, behaviorless; all behavior hangs off Database):
 *   DbEntity {
 *     char name[24];        // folded entity name (atom grammar)
 *     uint32_t pad;         // explicit padding
 *     Struct *schema;       // borrowed reflection layout (the entity)
 *     ChunkedList *rows;    // row pointers (void*); stable addresses
 *   }
 *
 * FUNCTION REGISTRY:
 * ----------------------------------------------------------------------------
 * Public Constructors: (.h) Database_0(), _1(chunkBytes), _init, _free
 * Public Core Functions: (.h) _kind, _check, _define, _entityIndex, _entityCount,
 *                             _schema, _insert, _row, _count, _select
 * Public Diagnostics: (.h) _errorCode, _lastError, _errorText
 * Public String Projections: (.h) _toString / _toStringStruct
 * ============================================================================
 */

// PRIVATE SLOT RECORD

typedef struct DbEntity {
    char name[DB_ENTITY_NAME_BYTES]; // folded entity name (atom grammar)
    uint32_t pad;                    // explicit padding
    Struct *schema;                  // borrowed reflection layout (the entity)
    ChunkedList *rows;               // row pointers (void*); stable addresses
} DbEntity;

// PRIVATE HELPERS

static void recordError(Database *self, int32_t code, const char *message) {
    if (self == nullptr)
        return;
    (*self).lastCode = code;
    if (message == nullptr) {
        (*self).lastError[0] = '\0';
        return;
    }
    size_t len = strlen(message);
    size_t cap = sizeof((*self).lastError);
    size_t copy = len < cap - 1u ? len : cap - 1u;
    memcpy((*self).lastError, message, copy);
    (*self).lastError[copy] = '\0';
}

static int32_t findEntity(const Database *self, const char *folded) {
    if (self == nullptr || folded == nullptr || (*self).entities == nullptr)
        return -1;
    uint32_t n = ChunkedList_size((*self).entities);
    for (uint32_t i = 0u; i < n; i++) {
        DbEntity *entity = (DbEntity*) ChunkedList_slot((*self).entities, i);
        if (entity != nullptr && strcmp((*entity).name, folded) == 0)
            return (int32_t) i;
    }
    return -1;
}

// CONSTRUCTORS

bool Database_init(Database *self, uint32_t chunkBytes) {
    if (self == nullptr)
        return false;
    memset(self, 0, sizeof(*self));
    uint32_t budget = chunkBytes == 0u ? VEX_CHUNKED_BYTES_DEFAULT : chunkBytes;
    if (budget < 16u)
        budget = VEX_CHUNKED_BYTES_DEFAULT;
    (*self).entities = ChunkedList_3(ID_DB_DATABASE, (uint32_t) sizeof(DbEntity), budget);
    if ((*self).entities == nullptr)
        return false;
    (*self).chunkBytes = budget;
    (*self).lastCode = DATABASE_OK;
    (*self).lastError[0] = '\0';
    return true;
}

Database *Database_1(uint32_t chunkBytes) {
    Database *self = (Database*) Memory_alloc(TYPE_DB_DATABASE_SINGLETON, sizeof(Database));
    if (self == nullptr)
        return nullptr;
    if (!Database_init(self, chunkBytes)) {
        Memory_free(self);
        return nullptr;
    }
    return self;
}

Database *Database_0(void) {
    return Database_1(VEX_CHUNKED_BYTES_DEFAULT);
}

void Database_free(Database *self) {
    if (self == nullptr)
        return;
    if ((*self).entities != nullptr) {
        uint32_t n = ChunkedList_size((*self).entities);
        for (uint32_t i = 0u; i < n; i++) {
            DbEntity *entity = (DbEntity*) ChunkedList_slot((*self).entities, i);
            if (entity != nullptr && (*entity).rows != nullptr)
                ChunkedList_free((*entity).rows);
        }
        ChunkedList_free((*self).entities);
        (*self).entities = nullptr;
    }
    Memory_free(self);
}

// CORE FUNCTIONS

uint64_t Database_kind(const Database *self) {
    if (self == nullptr)
        return 0u;
    return Memory_type((void*) self);
}

bool Database_check(const Database *self, uint64_t typeId) {
    if (self == nullptr)
        return false;
    return Memory_type((void*) self) == typeId;
}

int32_t Database_define(Database *self, Struct *schema) {
    if (self == nullptr || schema == nullptr) {
        recordError(self, DATABASE_INVALID, "define: null database or schema");
        THROW("Database_define: null database or schema");
        return -1;
    }
    char name[DB_ENTITY_NAME_BYTES];
    if (Struct_getName(schema, name, sizeof(name)) < 0) {
        recordError(self, DATABASE_INVALID, "define: schema has no valid name");
        THROW("Database_define: schema has no valid name");
        return -1;
    }
    char folded[DB_ENTITY_NAME_BYTES];
    if (!VariableSlot_foldName(name, folded)) {
        recordError(self, DATABASE_INVALID, "define: illegal entity name");
        THROW("Database_define: illegal entity name");
        return -1;
    }
    if (findEntity(self, folded) >= 0) {
        recordError(self, DATABASE_DUPLICATE, "define: entity already registered");
        THROW("Database_define: duplicate entity");
        return -1;
    }
    uint8_t *slot = ChunkedList_addSlot((*self).entities);
    if (slot == nullptr) {
        recordError(self, DATABASE_EXHAUSTED, "define: entity allocation failed");
        THROW("Database_define: entity allocation failed");
        return -1;
    }
    DbEntity *entity = (DbEntity*) slot;
    memcpy((*entity).name, folded, strlen(folded) + 1u);
    (*entity).schema = schema;
    (*entity).rows = ChunkedList_3(ID_DB_DATABASE_ROW, (uint32_t) sizeof(void*), (*self).chunkBytes);
    if ((*entity).rows == nullptr) {
        recordError(self, DATABASE_EXHAUSTED, "define: row list allocation failed");
        THROW("Database_define: row list allocation failed");
        return -1;
    }
    recordError(self, DATABASE_OK, "");
    return (int32_t) (ChunkedList_size((*self).entities) - 1u);
}

int32_t Database_entityIndex(const Database *self, const char *entity) {
    if (self == nullptr || entity == nullptr)
        return -1;
    char folded[DB_ENTITY_NAME_BYTES];
    if (!VariableSlot_foldName(entity, folded))
        return -1;
    return findEntity(self, folded);
}

uint32_t Database_entityCount(const Database *self) {
    if (self == nullptr || (*self).entities == nullptr)
        return 0u;
    return ChunkedList_size((*self).entities);
}

Struct *Database_schema(const Database *self, const char *entity) {
    int32_t index = Database_entityIndex(self, entity);
    if (index < 0)
        return nullptr;
    DbEntity *row = (DbEntity*) ChunkedList_slot((*self).entities, (uint32_t) index);
    return row ? (*row).schema : nullptr;
}

int32_t Database_insert(Database *self, const char *entity, void *row) {
    if (self == nullptr || entity == nullptr || row == nullptr) {
        recordError(self, DATABASE_INVALID, "insert: null argument");
        THROW("Database_insert: null argument");
        return -1;
    }
    char folded[DB_ENTITY_NAME_BYTES];
    if (!VariableSlot_foldName(entity, folded)) {
        recordError(self, DATABASE_INVALID, "insert: illegal entity name");
        THROW("Database_insert: illegal entity name");
        return -1;
    }
    int32_t index = findEntity(self, folded);
    if (index < 0) {
        recordError(self, DATABASE_ABSENT, "insert: no such entity");
        THROW("Database_insert: no such entity");
        return -1;
    }
    DbEntity *target = (DbEntity*) ChunkedList_slot((*self).entities, (uint32_t) index);
    if (target == nullptr || (*target).rows == nullptr) {
        recordError(self, DATABASE_ABSENT, "insert: entity unavailable");
        THROW("Database_insert: entity unavailable");
        return -1;
    }
    uint8_t *slot = ChunkedList_addSlot((*target).rows);
    if (slot == nullptr) {
        recordError(self, DATABASE_EXHAUSTED, "insert: row allocation failed");
        THROW("Database_insert: row allocation failed");
        return -1;
    }
    *((void**) slot) = row;
    recordError(self, DATABASE_OK, "");
    return (int32_t) (ChunkedList_size((*target).rows) - 1u);
}

void *Database_row(const Database *self, const char *entity, uint32_t index) {
    int32_t e = Database_entityIndex(self, entity);
    if (e < 0)
        return nullptr;
    DbEntity *target = (DbEntity*) ChunkedList_slot((*self).entities, (uint32_t) e);
    if (target == nullptr || (*target).rows == nullptr)
        return nullptr;
    uint8_t *slot = ChunkedList_slot((*target).rows, index);
    return slot ? *((void**) slot) : nullptr;
}

uint32_t Database_count(const Database *self, const char *entity) {
    int32_t e = Database_entityIndex(self, entity);
    if (e < 0)
        return 0u;
    DbEntity *target = (DbEntity*) ChunkedList_slot((*self).entities, (uint32_t) e);
    return target && (*target).rows ? ChunkedList_size((*target).rows) : 0u;
}

DatabaseResult *Database_select(Database *self, const char *entity) {
    int32_t e = Database_entityIndex(self, entity);
    if (e < 0) {
        recordError(self, DATABASE_ABSENT, "select: no such entity");
        THROW("Database_select: no such entity");
        return nullptr;
    }
    DbEntity *target = (DbEntity*) ChunkedList_slot((*self).entities, (uint32_t) e);
    if (target == nullptr || (*target).rows == nullptr) {
        recordError(self, DATABASE_ABSENT, "select: entity unavailable");
        THROW("Database_select: entity unavailable");
        return nullptr;
    }
    recordError(self, DATABASE_OK, "");
    return DatabaseResult_1((*target).rows);
}

// DIAGNOSTICS

int32_t Database_errorCode(const Database *self) {
    return self ? (*self).lastCode : DATABASE_INVALID;
}

int Database_lastError(const Database *self, char *out, size_t outCap) {
    if (self == nullptr || out == nullptr || outCap == 0u)
        return -1;
    size_t len = strlen((*self).lastError);
    size_t copy = len < outCap - 1u ? len : outCap - 1u;
    memcpy(out, (*self).lastError, copy);
    out[copy] = '\0';
    return (int) copy;
}

const char *Database_errorText(int32_t code) {
    switch (code) {
        case DATABASE_OK:        return "ok";
        case DATABASE_INVALID:   return "invalid argument";
        case DATABASE_DUPLICATE: return "duplicate entity";
        case DATABASE_ABSENT:    return "absent entity";
        case DATABASE_EXHAUSTED: return "allocation exhausted";
        default:                 return "unknown";
    }
}

// STRING PROJECTIONS (the toString Law)

void Database_toString(const Database *self, char *dest, size_t cap, bool *outTruncated) {
    if (outTruncated)
        *outTruncated = false;
    if (dest == nullptr || cap == 0u)
        return;
    if (self == nullptr) {
        snprintf(dest, cap, "nullptr");
        return;
    }
    int written = snprintf(dest, cap, "Database(entities=%u)", Database_entityCount(self));
    if (written < 0 || (size_t) written >= cap) {
        if (outTruncated)
            *outTruncated = true;
    }
}

void Database_toStringStruct(const Database *self, char *dest, size_t cap, bool *outTruncated) {
    if (outTruncated)
        *outTruncated = false;
    if (dest == nullptr || cap == 0u)
        return;
    if (self == nullptr) {
        snprintf(dest, cap, "nullptr");
        return;
    }
    int written = snprintf(dest, cap, "Database { entities=0x%llx, entityCount=%u, chunkBytes=%u, lastCode=%d }",
                           (unsigned long long) (uintptr_t) (*self).entities, Database_entityCount(self),
                           (*self).chunkBytes, (*self).lastCode);
    if (written < 0 || (size_t) written >= cap) {
        if (outTruncated)
            *outTruncated = true;
    }
}
