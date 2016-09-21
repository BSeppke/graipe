cmake_minimum_required(VERSION 3.1)

project(graipe_images)

#find . -type f -name \*.cxx | sed 's,^\./,,'
set(SOURCES 
	image.cxx
	imagebandparameter.cxx
	imageimpex.cxx
	imagesmodule.cxx
	imagestatistics.cxx
	imageviewcontroller.cxx)

#find . -type f -name \*.hxx | sed 's,^\./,,'
set(HEADERS  
	config.hxx
	geocoding.hxx
	image.hxx
	imagebandparameter.hxx
	imageimpex.hxx
	imagestatistics.hxx
	imageviewcontroller.hxx
    images.h)

add_definitions(-DGRAIPE_IMAGES_BUILD)
# Tell CMake to create the helloworld executable
add_library(graipe_images SHARED ${SOURCES} ${HEADERS})
set_target_properties(graipe_images PROPERTIES VERSION ${GRAIPE_VERSION} SOVERSION ${GRAIPE_SOVERSION})

# Link library to other libs
#
#target_link_libraries(graipe_images graipe_core ${GDAL_LIBRARY}  Qt5::Widgets)
target_link_libraries(graipe_images graipe_core ${GDAL_LIBRARY}  Qt5::Widgets)