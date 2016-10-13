cmake_minimum_required(VERSION 3.1)

project(graipe_core)

#find . -type f -name \*.cxx | sed 's,^\./,,'
set(SOURCES 
	algorithm.cxx
	colortables.cxx
	impex.cxx
	logging.cxx
	model.cxx
	module.cxx
	parameters/boolparameter.cxx
	parameters/colorparameter.cxx
	parameters/colortableparameter.cxx
	parameters/datetimeparameter.cxx
	parameters/doubleparameter.cxx
	parameters/enumparameter.cxx
	parameters/filenameparameter.cxx
	parameters/floatparameter.cxx
	parameters/intparameter.cxx
	parameters/longstringparameter.cxx
	parameters/modelparameter.cxx
	parameters/multimodelparameter.cxx
	parameters/parameter.cxx
	parameters/parametergroup.cxx
	parameters/pointfparameter.cxx
	parameters/pointparameter.cxx
	parameters/stringparameter.cxx
	parameters/transformparameter.cxx
	parameterselection.cxx
	qt_ext/qgraphicsresizableitem.cxx
	qt_ext/qiocompressor.cxx
	qt_ext/qlegend.cxx
	qt_ext/qpointfx.cxx
	serializable.cxx
	updatechecker.cxx
	viewcontroller.cxx)

#find . -type f -name \*.hxx | sed 's,^\./,,'
set(HEADERS  
	algorithm.hxx
	basicstatistics.hxx
	config.hxx
	colortables.hxx
	factories.hxx
	impex.hxx
	logging.hxx
	model.hxx
	module.hxx
	parameters/boolparameter.hxx
	parameters/colorparameter.hxx
	parameters/colortableparameter.hxx
	parameters/datetimeparameter.hxx
	parameters/doubleparameter.hxx
	parameters/enumparameter.hxx
	parameters/filenameparameter.hxx
	parameters/floatparameter.hxx
	parameters/intparameter.hxx
	parameters/longstringparameter.hxx
	parameters/modelparameter.hxx
	parameters/multimodelparameter.hxx
	parameters/parameter.hxx
	parameters/parametergroup.hxx
	parameters/pointfparameter.hxx
	parameters/pointparameter.hxx
	parameters/stringparameter.hxx
	parameters/transformparameter.hxx
	parameters.hxx
	parameterselection.hxx
	qt_ext/qgraphicsresizableitem.hxx
	qt_ext/qiocompressor.hxx
	qt_ext/qlegend.hxx
	qt_ext/qpointfx.hxx
	qt_ext.hxx
	serializable.hxx
	updatechecker.hxx
	viewcontroller.hxx
    core.h)
	

add_definitions(-DGRAIPE_CORE_BUILD)

# Tell CMake to create the library
add_library(graipe_core SHARED ${SOURCES} ${HEADERS})
set_target_properties(graipe_core PROPERTIES VERSION ${GRAIPE_VERSION} SOVERSION ${GRAIPE_SOVERSION})
target_link_libraries(graipe_core Qt5::Widgets Qt5::Network ${ZLIB_LIBRARIES})