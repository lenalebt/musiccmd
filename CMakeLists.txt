CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#project name
PROJECT(musiccmd)

#version number
SET (musiccmd_VERSION_MAJOR 0)
SET (musiccmd_VERSION_MINOR 8)

#tell the compiler to include the version number as preprocessor directive
ADD_DEFINITIONS(-DVERSION=${musiccmd_VERSION_MAJOR}.${musiccmd_VERSION_MINOR})

#this was mainly done to understand cmake
MESSAGE ( "Choose build type by typing \"cmake -DCMAKE_BUILD_TYPE=Debug\" or \"cmake -DCMAKE_BUILD_TYPE=Release\" or \"cmake -DCMAKE_BUILD_TYPE=Profiling\".")
IF (CMAKE_BUILD_TYPE STREQUAL Debug)
    MESSAGE("CMAKE_BUILD_TYPE set to Debug.")
ELSEIF (CMAKE_BUILD_TYPE STREQUAL Release)
    MESSAGE("CMAKE_BUILD_TYPE set to Release.")
    #für assert.h
    ADD_DEFINITIONS(-DNDEBUG)
ELSEIF (CMAKE_BUILD_TYPE STREQUAL Profiling)
    MESSAGE("CMAKE_BUILD_TYPE set to Profiling.")
    SET(CMAKE_CXX_FLAGS_PROFILING "-pg")
#    SET(CMAKE_EXE_LINKER_FLAGS_PROFILING "-pg")
    #for assert.h
    ADD_DEFINITIONS(-DNDEBUG)
ENDIF ()

#here, you would set some compiler specific things
IF ( CMAKE_COMPILER_IS_GNUCXX )
    MESSAGE ("Using Gnu g++ compiler...")
    #gcc should be more pedantic.
    ADD_DEFINITIONS(-Wall -pedantic -Wno-long-long)
ELSE ( MSVC )
    MESSAGE ("Using MS Visual Studio compiler...")
ENDIF()

#set cmake module directory, which is needed to find other libs
SET(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake/modules/"
    )

FIND_PACKAGE(Music REQUIRED)
#FIND_PACKAGE(SQLITE3 REQUIRED)
#used for matrix multiplication, maybe for fft, too
FIND_PACKAGE(Eigen3 3.1.2 REQUIRED)
#use doxygen for documentation. don't need this for compilation.
FIND_PACKAGE(Doxygen)

SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME OFF)

set(Boost_ADDITIONAL_VERSIONS "1.40.1" "1.44" "1.44.0" "1.46" "1.46.0" "1.46.1" "1.48" "1.48.0" "1.51" "1.51.1")
FIND_PACKAGE(Boost 1.46.1
    COMPONENTS
    program_options
    REQUIRED)

#place source files here
SET(
    musiccmd_SOURCES
    #src/
    src/main.cpp
    src/programoptions.cpp
    
    #src/db/
    src/db/addcontent.cpp
    src/db/editcontent.cpp
    src/db/removecontent.cpp
    src/db/querycontent.cpp
    
    #src/tools/
    src/tools/console_colors.cpp
    src/tools/logging.cpp
    src/tools/stringhelper.cpp
    src/tools/filesystem.cpp
    
    #src/tests/
    src/tests/debug.cpp
    src/tests/testframework.cpp
    src/tests/tests.cpp
    )

#place header here
SET(
    musiccmd_HEADERS
    #src/
    src/main.hpp
    src/programoptions.hpp
    
    #src/db/
    src/db/addcontent.hpp
    src/db/editcontent.hpp
    src/db/removecontent.hpp
    src/db/querycontent.hpp
    
    #src/tools/
    src/tools/console_colors.hpp
    src/tools/logging.hpp
    src/tools/stringhelper.hpp
    src/tools/filesystem.hpp
    
    #src/tests/
    src/tests/debug.hpp
    src/tests/testframework.hpp
    src/tests/tests.hpp
    )

#put all (possibly autocreated) headers and source files here
SET(
    musiccmd_ALL_SOURCES
    ${musiccmd_SOURCES}
    )

SET(
    musiccmd_ALL_HEADERS
    ${musiccmd_HEADERS}
    )    

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${SQLITE3_INCLUDE_DIR}
    ${MUSIC_INCLUDE_DIR}
    ${EIGEN3_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
    src/
    src/tests/
    src/tools/
    src/db/
    )

#to copy a directory (e.g. test files) to another.
#FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/testfiledirectory DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

IF(DOXYGEN_FOUND)
    #convert semicolon-seperated list to whitespace-seperated one
    FOREACH(ARG ${musiccmd_HEADERS})
        SET(musiccmd_HEADERS_WHITESPACE "${musiccmd_HEADERS_WHITESPACE} ${CMAKE_CURRENT_SOURCE_DIR}/${ARG}")
    ENDFOREACH(ARG ${musiccmd_HEADERS})
    FOREACH(ARG ${musiccmd_SOURCES})
        SET(musiccmd_SOURCES_WHITESPACE "${musiccmd_SOURCES_WHITESPACE} ${CMAKE_CURRENT_SOURCE_DIR}/${ARG}")
    ENDFOREACH(ARG ${musiccmd_SOURCES})
    GET_DIRECTORY_PROPERTY(DIRINC INCLUDE_DIRECTORIES)
    FOREACH(ARG ${DIRINC})
        SET(musiccmd_INCLUDES_WHITESPACE "${musiccmd_INCLUDES_WHITESPACE} ${ARG}")
    ENDFOREACH(ARG ${DIRINC})
    
    #add configuration file
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    #add doxygen as new target
    ADD_CUSTOM_TARGET(doxygen ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    #"make clean" should also clean doxygen
    SET_PROPERTY(DIRECTORY APPEND PROPERTY
	     ADDITIONAL_MAKE_CLEAN_FILES api-doc)
    GET_TARGET_PROPERTY(DOC_TARGET doc TYPE)
    IF(NOT DOC_TARGET)
        ADD_CUSTOM_TARGET(doc)
    ENDIF()
    ADD_DEPENDENCIES(doc doxygen)
ENDIF()

#add executable files for our project
ADD_EXECUTABLE(
    musiccmd
    ${musiccmd_ALL_SOURCES}
    ${musiccmd_ALL_HEADERS_MOC}
    )

#add unit tests here
ENABLE_TESTING()
ADD_TEST(basename                  "musiccmd" "--test" "basename")
ADD_TEST(torelativepath            "musiccmd" "--test" "torelativepath")
ADD_TEST(stringhelper              "musiccmd" "--test" "stringhelper")
#use "make test" to start all tests.

#use this to automatically create installation packages for different platforms
IF(UNIX)
    SET(CPACK_SOURCE_GENERATOR
        TGZ
        TBZ2
    )
    SET(CPACK_GENERATOR
        TGZ
        TBZ2
        RPM
        DEB
    )
ELSE(UNIX)
    SET(CPACK_SOURCE_GENERATOR
        ZIP
    )
    SET(CPACK_GENERATOR
        NSIS
    )
ENDIF(UNIX)

#meta data
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "musiccmd is a MUsic SImilarity Classifier"
    )
SET(CPACK_PACKAGE_VENDOR
    "-"
    )
SET(CPACK_PACKAGE_DESCRIPTION_FILE
    #"${CMAKE_CURRENT_SOURCE_DIR}/README.TXT"
    )
SET(CPACK_RESOURCE_FILE_LICENSE
    #"${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.TXT"
    )
SET(CPACK_PACKAGE_VERSION_MAJOR
    ${musiccmd_VERSION_MAJOR}
    )
SET(CPACK_PACKAGE_VERSION_MINOR
    ${musiccmd_VERSION_MINOR}
    )
SET(CPACK_PACKAGE_CONTACT
    "lbruder@rim.com"
    )
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER
    "Lena Brueder"
    )
#maybe set something better here
SET(CPACK_PACKAGE_SECTION
    "musiccmd"
    )
SET(CPACK_SOURCE_IGNORE_FILES
    ${CMAKE_BINARY_DIR}/*
    ${CMAKE_CURRENT_BINARY_DIR}/*
    ${CMAKE_CURRENT_SOURCE_DIR}/.gitignore
    ${CMAKE_CURRENT_SOURCE_DIR}/.git
    ${CMAKE_CURRENT_SOURCE_DIR}/build/*
    )
INCLUDE(CPack)


#tell the linker which libs need to be linked
TARGET_LINK_LIBRARIES(musiccmd
    ${SQLITE3_LIBRARIES}
    ${MUSIC_LIBRARIES}
    ${Boost_LIBRARIES}
    m
    )

INSTALL (TARGETS musiccmd
    DESTINATION bin
    )

