
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/include )

# Source Files
SET(Printer_headers	include/ModelicaPrinter.h
					include/PythonPrinter.h
					include/MatlabPrinter.h
					include/MatlabSymPrinter.h
					include/FortranPrinter.h
					include/CPrinter.h
					include/FMUPrinter.h
                    include/Printer.h)

SET(Printer_sources	ModelicaPrinter.cpp
					PythonPrinter.cpp
					MatlabPrinter.cpp
					MatlabSymPrinter.cpp
					FortranPrinter.cpp
					CPrinter.cpp
					FMUPrinter.cpp
                    Printer.cpp)

# Target
IF(WIN32)
ELSE()
	ADD_DEFINITIONS(-fPIC)
ENDIF()
ADD_LIBRARY( Printer STATIC ${Printer_sources} ${Printer_headers} )
ADD_DEPENDENCIES( Printer Symbolics Graph )
TARGET_LINK_LIBRARIES( Printer Symbolics Graph)

ADD_SUBDIRECTORY( Test )
