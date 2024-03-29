# Find libmusic
#
# MUSIC_INCLUDE_DIR - where to find MUSIC.hpp, etc.
# MUSIC_LIBRARIES - List of libraries when using libMUSIC.
# MUSIC_FOUND - True if libMUSIC found.

IF(MUSIC_INCLUDE_DIR)
    # be silent, file already in cache
    SET(MUSIC_FIND_QUIETLY TRUE)
ENDIF(MUSIC_INCLUDE_DIR)

FIND_PATH(MUSIC_INCLUDE_DIR music.hpp PATHS
        ${CMAKE_INSTALL_PREFIX}/include)

FIND_LIBRARY(MUSIC_LIBRARY NAMES music PATHS
        /usr/lib /usr/local/lib /armle-v7/usr/lib /armle-v7/lib)

# Handle the QUIETLY and REQUIRED arguments and set MUSIC_FOUND to TRUE if
# all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MUSIC DEFAULT_MSG MUSIC_LIBRARY MUSIC_INCLUDE_DIR)

IF(MUSIC_FOUND)
  SET(MUSIC_LIBRARIES ${MUSIC_LIBRARY})
ELSE(MUSIC_FOUND)
  SET(MUSIC_LIBRARIES)
  IF(REQUIRED)
    MESSAGE(FATAL_ERROR "Required library music not found.")
  ENDIF(REQUIRED)
ENDIF(MUSIC_FOUND)

MARK_AS_ADVANCED(MUSIC_INCLUDE_DIR MUSIC_LIBRARY)
