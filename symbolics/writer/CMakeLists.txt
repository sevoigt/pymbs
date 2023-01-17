
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/include )

# Source Files
SET(Writer_headers	include/ModelicaWriter.h
					include/PythonWriter.h
					include/MatlabWriter.h
					include/FortranWriter.h
					include/CWriter.h
					include/CSharpWriter.h
					include/FMUWriter.h
					lib/lib_xml_writer.h
                    include/Writer.h)

SET(Writer_sources	ModelicaWriter.cpp
					PythonWriter.cpp
					MatlabWriter.cpp
					FortranWriter.cpp
					CWriter.cpp
					CSharpWriter.cpp
					FMUWriter.cpp
                    Writer.cpp)

# Target
IF(WIN32)
ELSE()
	ADD_DEFINITIONS(-fPIC)
ENDIF()
ADD_LIBRARY( Writer STATIC ${Writer_sources} ${Writer_headers} )
ADD_DEPENDENCIES( Writer Symbolics Printer Functions )
TARGET_LINK_LIBRARIES( Writer Symbolics Functions Printer ${Boost_FILESYSTEM_LIBRARY} )


ADD_SUBDIRECTORY( test )
