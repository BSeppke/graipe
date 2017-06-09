cmake_minimum_required(VERSION 3.1)

project(GaripeClient)

#find . -type f -name \*.cxx | sed 's,^\./,,'
set(SOURCES 
	main.cpp
	client.cxx)

#find . -type f -name \*.hxx | sed 's,^\./,,'
set(HEADERS 
	client.hxx)

#--------------------------------------------------------------------------------
#  CMake's way of creating an executable
add_executable(GraipeClient MACOSX_BUNDLE WIN32 ${SOURCES} ${HEADERS})

# Link executable to other libs

target_link_libraries(GraipeClient graipe_core  Qt5::Widgets Qt5::Network)