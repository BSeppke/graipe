cmake_minimum_required(VERSION 3.1)

set (CMAKE_CXX_STANDARD 11)

include_directories(. ..)

set(GRAIPE_BIN_DIR  ${CMAKE_CURRENT_LIST_DIR}/../bin)

set(GRAIPE_VERSION_MAJOR 1)
set(GRAIPE_VERSION_MINOR 0)
set(GRAIPE_VERSION_PATCH 0)

execute_process(COMMAND git describe --dirty --always --tags OUTPUT_VARIABLE GRAIPE_GIT_RESPONSE)
string(STRIP ${GRAIPE_GIT_RESPONSE} GRAIPE_GIT_VERSION)

add_definitions(-DGRAIPE_VERSION_MAJOR=${GRAIPE_VERSION_MAJOR})
add_definitions(-DGRAIPE_VERSION_MINOR=${GRAIPE_VERSION_MINOR})
add_definitions(-DGRAIPE_VERSION_PATCH=${GRAIPE_VERSION_PATCH})
add_definitions(-DGRAIPE_GIT_VERSION="${GRAIPE_GIT_VERSION}")

set(GRAIPE_VERSION ${GRAIPE_VERSION_MAJOR}.${GRAIPE_VERSION_MINOR}.${GRAIPE_VERSION_PATCH})
set(GRAIPE_SOVERSION ${GRAIPE_VERSION_MAJOR})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GRAIPE_BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GRAIPE_BIN_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${GRAIPE_BIN_DIR})

foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
	string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${GRAIPE_BIN_DIR})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${GRAIPE_BIN_DIR})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${GRAIPE_BIN_DIR})
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPE)

set(CMAKE_MACOSX_RPATH FALSE)

# Find ZLIB
find_package(ZLIB)
if (ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
endif()

# Find VIGRA
find_package(Vigra)
if(Vigra_FOUND)
    include_directories(${Vigra_INCLUDE_DIRS})
endif()

# Find GDAL
find_package(GDAL)
if(GDAL_FOUND)
    include_directories(${GDAL_INCLUDE_DIR})
endif()

# Find the FFTW lib
find_library(FFTW_LIBRARY
     NAMES fftw-3.3 fftw3 fftw fftwf-3.3 fftwf3 fftwf)
set(FFTW_LIBRARIES "${FFTW_LIBRARY}")
if(UNIX AND NOT WIN32)
     find_library(FFTW_libm_LIBRARY
         NAMES m)
     list(APPEND FFTW_LIBRARIES "${FFTW_libm_LIBRARY}")
endif()

# Find the FFTWF lib
find_library(FFTWF_LIBRARY
     NAMES fftw-3.3f fftwf-3.3 fftw3f fftwf)
list(APPEND FFTW_LIBRARIES "${FFTWF_LIBRARY}")

# Find the Qt5 modules needed
find_package(Qt5Widgets)
find_package(Qt5Network)
find_package(Qt5PrintSupport)
find_package(Qt5Svg)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Instruct CMake to run uic automatically when needed.
set(CMAKE_AUTOUIC ON)

# Use current build dir as well for includes
set(CMAKE_INCLUDE_CURRENT_DIR ON)