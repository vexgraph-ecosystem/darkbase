#ifndef DARKBASE_TYPE_H
#define DARKBASE_TYPE_H

#include <stdint.h>

#include "oop/type.h"

// darkbase/type.h — the darkbase class registry (the One Type Registry Law).
//
// Every darkbase class is stamped with a 64-bit id built by Type_make from the
// PROJ_DARKBASE project byte, a form, and the class number below. This is the
// single source of truth for darkbase's ids; no other header may redefine them.
//
// Project byte: PROJ_DARKBASE (0x06) is granted in vexspoke's oop/type.h and
// resolved to ARCH_DARKBASE by Type_arch — the same per-project numbering every
// repo uses. Class numbers start at 1 with gaps allowed.
//
// NAMING: this header includes oop/type.h, so the preprocessor namespace is
// shared with vexspoke's bare ID_* macros (for example vexspoke already owns
// ID_ENTITY and ID_VARIABLE). Darkbase therefore prefixes every macro with DB_
// (ID_DB_*, TYPE_DB_*) so no registry symbol can collide with another project.
// Bare ID_* constants name vexspoke's own class space only; cross-project
// dispatch ships the full TYPE_DB_* ids below.
//
// The entity model maps C vocabulary to store vocabulary:
//   struct   -> Entity          (a table: an ordered EntityField list)
//   field    -> EntityField     (a physical column: name, offset, size, typeId)
//   function -> EntityFunction  (a named callable binding)
//   trigger  -> DbTrigger       (a reactive program bound to a store event)
//
// M0 registers the ids. Behavior lands per class in later milestones; an id
// present here is a registry entry, not an implementation claim.

#define ID_DB_DATABASE           0x0001u
#define ID_DB_DATABASE_RESULT    0x0002u
#define ID_DB_DATABASE_ROW       0x0003u
#define ID_DB_DATABASE_ERROR     0x0004u
#define ID_DB_DRIVER             0x0005u
#define ID_DB_URI                0x0006u
#define ID_DB_SCHEMA             0x0007u
#define ID_DB_ENTITY             0x0008u
#define ID_DB_ENTITY_FIELD       0x0009u
#define ID_DB_ENTITY_FUNCTION    0x000Au
#define ID_DB_TRIGGER            0x000Bu

#define TYPE_DB_DATABASE_SINGLETON        (SUGAR_VEX | PROJ_DARKBASE | FORM_STRUCT_SINGLETON | ID_DB_DATABASE)
#define TYPE_DB_DATABASE_RESULT_SINGLETON (SUGAR_VEX | PROJ_DARKBASE | FORM_STRUCT_SINGLETON | ID_DB_DATABASE_RESULT)
#define TYPE_DB_DATABASE_ROW_SINGLETON    (SUGAR_VEX | PROJ_DARKBASE | FORM_STRUCT_SINGLETON | ID_DB_DATABASE_ROW)
#define TYPE_DB_DATABASE_ERROR_SINGLETON  (SUGAR_VEX | PROJ_DARKBASE | FORM_STRUCT_SINGLETON | ID_DB_DATABASE_ERROR)
#define TYPE_DB_DRIVER_SINGLETON          (SUGAR_VEX | PROJ_DARKBASE | FORM_STRUCT_SINGLETON | ID_DB_DRIVER)
#define TYPE_DB_URI_SINGLETON             (SUGAR_VEX | PROJ_DARKBASE | FORM_STRUCT_SINGLETON | ID_DB_URI)
#define TYPE_DB_SCHEMA_SINGLETON          (SUGAR_VEX | PROJ_DARKBASE | FORM_STRUCT_SINGLETON | ID_DB_SCHEMA)
#define TYPE_DB_ENTITY_SINGLETON          (SUGAR_VEX | PROJ_DARKBASE | FORM_STRUCT_SINGLETON | ID_DB_ENTITY)
#define TYPE_DB_ENTITY_FIELD_SINGLETON    (SUGAR_VEX | PROJ_DARKBASE | FORM_STRUCT_SINGLETON | ID_DB_ENTITY_FIELD)
#define TYPE_DB_ENTITY_FUNCTION_SINGLETON (SUGAR_VEX | PROJ_DARKBASE | FORM_STRUCT_SINGLETON | ID_DB_ENTITY_FUNCTION)
#define TYPE_DB_TRIGGER_SINGLETON         (SUGAR_VEX | PROJ_DARKBASE | FORM_STRUCT_SINGLETON | ID_DB_TRIGGER)

#endif
