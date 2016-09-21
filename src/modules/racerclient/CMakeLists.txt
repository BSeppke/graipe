cmake_minimum_required(VERSION 3.1)

project(graipe_racerclient)

set(SOURCES 
	racerclientmodule.cxx
	racerconnection.cxx)

set(HEADERS
    config.hxx
	racerclient.h
	racerconnection.hxx)

add_definitions(-DGRAIPE_RACERCLIENT_BUILD)

# Tell CMake to create the library
add_library(graipe_racerclient SHARED ${SOURCES} ${HEADERS})
set_target_properties(graipe_racerclient PROPERTIES VERSION ${GRAIPE_VERSION} SOVERSION ${GRAIPE_SOVERSION})

# Link library to other libs
target_link_libraries(graipe_racerclient graipe_core graipe_features graipe_images graipe_vectorfields ${FFTW_LIBRARY} Qt5::Widgets Qt5::Network)