cmake_minimum_required(VERSION 3.1)

project(Graipe)

#find . -type f -name \*.cxx | sed 's,^\./,,'
set(SOURCES 
	main.cpp
	mainwindow.cxx
	qlistwidgetitems.cxx
	statuswindow.cxx)

#find . -type f -name \*.hxx | sed 's,^\./,,'
set(HEADERS 
	mainwindow.hxx
	memorystatus.hxx
	qlistwidgetitems.hxx
	statuswindow.hxx)

#--------------------------------------------------------------------------------
# For Apple set the icns file containing icons
IF(APPLE)
  # set how it shows up in the Info.plist file
  SET(MACOSX_BUNDLE_ICON_FILE graipe.icns) 
  # set where in the bundle to put the icns file
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/icon/graipe.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  SET(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/icon/graipe.icns)
ENDIF(APPLE)

#--------------------------------------------------------------------------------
# For Apple set the icns file containing icons
IF(WIN32)    
  set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/icon/graipe.rc ${SOURCES})
ENDIF(WIN32)

#--------------------------------------------------------------------------------
#  CMake's way of creating an executable
add_executable(Graipe MACOSX_BUNDLE WIN32 ${SOURCES} ${UI_FILES} ${HEADERS})

# Link executable to other libs

target_link_libraries(Graipe graipe_core  Qt5::Widgets Qt5::PrintSupport Qt5::Svg)