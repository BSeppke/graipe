cmake_minimum_required(VERSION 3.1)

project(graipe_analysis)

set(SOURCES 
	analysismodule.cxx)

set(HEADERS  
	analysis.h
	vectorfieldcomparison.hxx
	vectorfieldseparation.hxx)

add_definitions(-DGRAIPE_ANALYSIS_BUILD)

# Tell CMake to create the library
add_library(graipe_analysis SHARED ${SOURCES} ${HEADERS})
set_target_properties(graipe_analysis PROPERTIES VERSION ${GRAIPE_VERSION} SOVERSION ${GRAIPE_SOVERSION})

# Link library to other libs
#
##find_library(GRAIPE_VECTORFIELDS_LIBRARY graipe_vectorfields PATHS ${GRAIPE_BIN_DIR})
#target_link_libraries(graipe_analysis graipe_core graipe_features graipe_vectorfields Qt5::Widgets)
target_link_libraries(graipe_analysis graipe_core graipe_features graipe_vectorfields Qt5::Widgets)
