cmake_minimum_required(VERSION 3.1)

project(graipe_features)

#find . -type f -name \*.cxx | sed 's,^\./,,'
set(SOURCES 
	cubicspline.cxx
	cubicsplinelist.cxx
	cubicsplineliststatistics.cxx
	cubicsplinelistviewcontroller.cxx
	featurelist.cxx
	featureliststatistics.cxx
	featurelistviewcontroller.cxx
	features2dmodule.cxx
	polygon.cxx
	polygonlist.cxx
	polygonliststatistics.cxx
	polygonlistviewcontroller.cxx)

#find . -type f -name \*.hxx | sed 's,^\./,,'
set(HEADERS  
	config.hxx
	cubicspline.hxx
	cubicsplinelist.hxx
	cubicsplineliststatistics.hxx
	cubicsplinelistviewcontroller.hxx
	featurelist.hxx
	featureliststatistics.hxx
	featurelistviewcontroller.hxx
	polygon.hxx
	polygonlist.hxx
	polygonliststatistics.hxx
	polygonlistviewcontroller.hxx
    features2d.h)

add_definitions(-DGRAIPE_FEATURES_BUILD)

# Tell CMake to create the library
add_library(graipe_features SHARED ${SOURCES} ${HEADERS})
set_target_properties(graipe_features PROPERTIES VERSION ${GRAIPE_VERSION} SOVERSION ${GRAIPE_SOVERSION})

# Link library to other libs
#
#target_link_libraries(graipe_features graipe_core Qt5::Widgets)
target_link_libraries(graipe_features graipe_core Qt5::Widgets)
