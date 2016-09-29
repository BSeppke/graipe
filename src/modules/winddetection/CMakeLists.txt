cmake_minimum_required(VERSION 3.1)

project(graipe_winddetection)

set(SOURCES 
	winddetectionmodule.cxx)

set(HEADERS  
	winddetection.h
	detectwindpatterns.hxx)

add_definitions(-DGRAIPE_WINDDETECTION_BUILD)

# Tell CMake to create the library
add_library(graipe_winddetection SHARED ${SOURCES} ${HEADERS})
set_target_properties(graipe_winddetection PROPERTIES VERSION ${GRAIPE_VERSION} SOVERSION ${GRAIPE_SOVERSION})

# Link library to other libs
target_link_libraries(graipe_winddetection graipe_core graipe_features2d graipe_images graipe_vectorfields ${FFTW_LIBRARIES} Qt5::Widgets)
