cmake_minimum_required(VERSION 3.1)

project(graipe_multispectral)

set(SOURCES 
	multispectralmodule.cxx)

set(HEADERS  
	multispectral.h
	multispectralclassification.hxx
	multispectralgradients.hxx
	multispectralopticalflow.hxx
	multispectralopticalflowalgorithms.hxx
	multispectralopticalflowframework.hxx)

add_definitions(-DGRAIPE_MULTISPECTRAL_BUILD)

# Tell CMake to create the library
add_library(graipe_multispectral SHARED ${SOURCES} ${HEADERS})
set_target_properties(graipe_multispectral PROPERTIES VERSION ${GRAIPE_VERSION} SOVERSION ${GRAIPE_SOVERSION})

# Link library to other libs
target_link_libraries(graipe_multispectral graipe_core graipe_features graipe_images graipe_vectorfields ${FFTW_LIBRARY} Qt5::Widgets)