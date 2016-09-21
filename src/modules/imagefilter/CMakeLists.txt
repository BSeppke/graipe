cmake_minimum_required(VERSION 3.1)

project(graipe_imagefilter)

set(SOURCES 
	imagefiltermodule.cxx)

set(HEADERS  
	imagefilter.h)

add_definitions(-DGRAIPE_IMAGEFILTER_BUILD)

# Tell CMake to create the library
add_library(graipe_imagefilter SHARED ${SOURCES} ${HEADERS})
set_target_properties(graipe_imagefilter PROPERTIES VERSION ${GRAIPE_VERSION} SOVERSION ${GRAIPE_SOVERSION})

# Link library to other libs

target_link_libraries(graipe_imagefilter graipe_core graipe_images Qt5::Widgets)