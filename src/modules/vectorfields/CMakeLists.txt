cmake_minimum_required(VERSION 3.1)

project(graipe_vectorfields)

#find . -type f -name \*.cxx | sed 's,^\./,,'
set(SOURCES
	densevectorfield.cxx
	densevectorfieldstatistics.cxx
	densevectorfieldviewcontroller.cxx
	densevectorfieldimpex.cxx
	sparsevectorfield.cxx
	sparsevectorfieldstatistics.cxx
	sparsevectorfieldviewcontroller.cxx
	vectordrawer.cxx
	vectorfield.cxx
    vectorfieldsmodule.cxx)

#find . -type f -name \*.hxx | sed 's,^\./,,'
set(HEADERS  
	config.hxx
	densevectorfield.hxx
	densevectorfieldstatistics.hxx
	densevectorfieldviewcontroller.hxx
	densevectorfieldimpex.hxx
	sparsevectorfield.hxx
	sparsevectorfieldstatistics.hxx
	sparsevectorfieldviewcontroller.hxx
	vectordrawer.hxx
	vectorfield.hxx
	vectorfields.h)

add_definitions(-DGRAIPE_VECTORFIELDS_BUILD)

# Tell CMake to create the library
add_library(graipe_vectorfields SHARED ${SOURCES} ${HEADERS})
set_target_properties(graipe_vectorfields PROPERTIES VERSION ${GRAIPE_VERSION} SOVERSION ${GRAIPE_SOVERSION})

# Link to other libs
#
#target_link_libraries(graipe_vectorfields graipe_core ${GDAL_LIBRARY}  Qt5::Widgets)
target_link_libraries(graipe_vectorfields graipe_core ${GDAL_LIBRARY}  Qt5::Widgets)