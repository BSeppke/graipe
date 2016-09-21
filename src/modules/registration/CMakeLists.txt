cmake_minimum_required(VERSION 3.1)

project(graipe_registration)

set(SOURCES 
	registrationmodule.cxx)

set(HEADERS
	warpingfunctors.hxx
    piecewiseaffine_registration.hxx
    delaunay.hxx)

add_definitions(-DGRAIPE_REGISTRATION_BUILD)

# Tell CMake to create the library
add_library(graipe_registration SHARED ${SOURCES} ${HEADERS})
set_target_properties(graipe_registration PROPERTIES VERSION ${GRAIPE_VERSION} SOVERSION ${GRAIPE_SOVERSION})

# Link library to other libs
target_link_libraries(graipe_registration graipe_core graipe_features graipe_images graipe_vectorfields ${FFTW_LIBRARY} Qt5::Widgets)