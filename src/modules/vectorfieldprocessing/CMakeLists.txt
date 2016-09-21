cmake_minimum_required(VERSION 3.1)

project(graipe_vectorfieldprocessing)

set(SOURCES 
	vectorfieldprocessingmodule.cxx)

set(HEADERS  
	vectorfieldprocessing.h
	vectorclustering.hxx
	vectorsmoothing.hxx)

add_definitions(-DGRAIPE_VECTORFIELDPROCESSING_BUILD)

# Tell CMake to create the library
add_library(graipe_vectorfieldprocessing SHARED ${SOURCES} ${HEADERS})
set_target_properties(graipe_vectorfieldprocessing PROPERTIES VERSION ${GRAIPE_VERSION} SOVERSION ${GRAIPE_SOVERSION})

# Link library to other libs
target_link_libraries(graipe_vectorfieldprocessing graipe_core graipe_features graipe_images graipe_vectorfields Qt5::Widgets)
