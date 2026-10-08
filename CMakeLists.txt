cmake_minimum_required(VERSION 3.24)
# IDE metadata only: the registry header exists, but no production .c source
# targets exist yet; do not invent them. Real builds belong to b.
project(darkbase_ide LANGUAGES NONE)
message(STATUS "IDE metadata only: src/darkbase/type.h registry present; no .c targets yet. Future builds belong to b, not this file.")
