cmake_minimum_required(VERSION 3.1)

project(graipe_featuredetection)

set(SOURCES 
	featuredetectionmodule.cxx)

set(HEADERS  
	featuredetection.h
	detectpointfeatures.hxx
	detectsiftfeatures.hxx
    sift.hxx)

add_definitions(-DGRAIPE_FEATUREDETECTION_BUILD)

# Tell CMake to create the library
add_library(graipe_featuredetection SHARED ${SOURCES} ${HEADERS})
set_target_properties(graipe_featuredetection PROPERTIES VERSION ${GRAIPE_VERSION} SOVERSION ${GRAIPE_SOVERSION})

# Link library to other libs

target_link_libraries(graipe_featuredetection graipe_core graipe_features graipe_images Qt5::Widgets)
