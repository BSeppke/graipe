cmake_minimum_required(VERSION 3.1)

project(graipe_imageprocessing)

set(SOURCES 
	imageprocessingmodule.cxx)

set(HEADERS  
	imageprocessing.h)

add_definitions(-DGRAIPE_IMAGEPROCESSING_BUILD)

# Tell CMake to create the library
add_library(graipe_imageprocessing SHARED ${SOURCES} ${HEADERS})
set_target_properties(graipe_imageprocessing PROPERTIES VERSION ${GRAIPE_VERSION} SOVERSION ${GRAIPE_SOVERSION})

# Link library to other libs
target_link_libraries(graipe_imageprocessing graipe_core graipe_features graipe_images graipe_vectorfields Qt5::Widgets)