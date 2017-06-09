cmake_minimum_required(VERSION 3.1)

project(GraipeServer)

#find . -type f -name \*.cxx | sed 's,^\./,,'
set(SOURCES 
	main.cpp
	maindialog.cxx
	server.cxx
	workerthread.cxx)

#find . -type f -name \*.hxx | sed 's,^\./,,'
set(HEADERS 
	maindialog.hxx
	server.hxx
	workerthread.hxx)

#--------------------------------------------------------------------------------
#  CMake's way of creating an executable
add_executable(GraipeServer MACOSX_BUNDLE WIN32 ${SOURCES} ${HEADERS})

# Link executable to other libs

target_link_libraries(GraipeServer graipe_core  Qt5::Widgets Qt5::Network)