cmake_minimum_required(VERSION 3.1)

project(graipe_opticalflow)

set(SOURCES 
	opticalflowmodule.cxx)

set(HEADERS  
	opticalflow.h
	opticalflow_experimental.hxx
	opticalflow_global.hxx
	opticalflow_hybrid.hxx
	opticalflow_local.hxx
	opticalflowalgorithms.hxx
	opticalflowframework.hxx
	opticalflowgradients.hxx)

add_definitions(-DGRAIPE_OPTICALFLOW_BUILD)

# Tell CMake to create the library
add_library(graipe_opticalflow SHARED ${SOURCES} ${HEADERS})
set_target_properties(graipe_opticalflow PROPERTIES VERSION ${GRAIPE_VERSION} SOVERSION ${GRAIPE_SOVERSION})

# Link library to other libs
target_link_libraries(graipe_opticalflow graipe_core graipe_features2d graipe_images graipe_vectorfields ${FFTW_LIBRARY} Qt5::Widgets)
