# Python
INCLUDE_DIRECTORIES( ${Python_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/include )

MESSAGE("Python include dirs: " ${Python_INCLUDE_DIRS} )

# Source Files
SET( SympyWrapper_headers   include/CBasic.h
                            include/CAssignment.h
                            include/CSymbol.h
                            include/CGraph.h
							include/CMatrix.h
                            include/PySymbolics.h
                            include/SymbolicsError.h
                            include/convert.h
							include/CSin.h
							include/CAcos.h
							include/CAsin.h
							include/CAtan.h
							include/CAtan2.h
							include/CCos.h
							include/CDer.h
							include/CElement.h
							include/CNumber.h
							include/CScalar.h
							include/CSkew.h
							include/CSolve.h
							include/CTan.h
							include/CTranspose.h
							include/CZeros.h
							include/CEye.h
							include/COuter.h
							include/PyFactory.h
							include/CIf.h
							include/CSign.h
							include/CJacobian.h
							include/CInverse.h
							include/CLess.h
							include/CGreater.h
							include/CEqual.h)

SET( SympyWrapper_sources  	CBasic.cpp
                            CAssignment.cpp
							CSymbol.cpp
                            CGraph.cpp
							CMatrix.cpp
							CNumber.cpp
                            PySymbolics.cpp
                            SymbolicsError.cpp
                            convert.cpp
							CSin.cpp
							CAcos.cpp
							CAsin.cpp
							CAtan.cpp
							CAtan2.cpp
							CCos.cpp
							CDer.cpp
							CElement.cpp
							CScalar.cpp
							CSkew.cpp
							CSolve.cpp
							CTan.cpp
							CTranspose.cpp
							CZeros.cpp
							CEye.cpp
							COuter.cpp
							PyFactory.cpp
							CIf.cpp
							CSign.cpp
							CJacobian.cpp
							CInverse.cpp
							CLess.cpp
							CGreater.cpp
							CEqual.cpp
                            Symbolics.def)

# Target

IF(WIN32)
ELSE()
	ADD_DEFINITIONS(-fPIC)
ENDIF()

ADD_LIBRARY( SympyWrapper SHARED ${SympyWrapper_headers} ${SympyWrapper_sources} )
ADD_DEPENDENCIES( SympyWrapper Symbolics Functions Graph Writer )
TARGET_LINK_LIBRARIES( SympyWrapper Symbolics Functions Graph Printer Writer ${Python_LIBRARIES}) #${PYTHON_LIBRARIES} )
SET_TARGET_PROPERTIES( SympyWrapper PROPERTIES     OUTPUT_NAME "symbolics" )
IF(WIN32)
    SET_TARGET_PROPERTIES( SympyWrapper PROPERTIES     SUFFIX ".pyd" )
ELSE(WIN32)
    SET_TARGET_PROPERTIES( SympyWrapper PROPERTIES     PREFIX "" )
ENDIF(WIN32)

INSTALL( TARGETS SympyWrapper DESTINATION ${CMAKE_CURRENT_SOURCE_DIR} )

IF(MSVC)
    INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/symbolics.pdb DESTINATION ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURATIONS Debug )
    INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/symbolics.pdb DESTINATION ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURATIONS RelWithDebInfo )
ENDIF(MSVC)

PYTHONTEST( test_symbolics ${CMAKE_CURRENT_SOURCE_DIR})
