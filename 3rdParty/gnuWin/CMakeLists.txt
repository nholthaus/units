# ------------------------------------------------------------------------------------------------------------------------------
# CMAKE SPECIFIC INFO. Versions/paths/etc go here.
# ------------------------------------------------------------------------------------------------------------------------------

# Version 3 is required to find some of the modules
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

LIST(APPEND CMAKE_PROGRAM_PATH  ${CMAKE_CURRENT_SOURCE_DIR})
find_program(PATCH NAMES patch patch.exe applydiff.exe)
find_program(SED NAMES sed sed.exe)