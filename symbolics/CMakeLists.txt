CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(Symbolics)


# gcc from 4.7 is a bit uncooperativ with forward declarations and templated
IF(NOT WIN32)
  ADD_DEFINITIONS("-fpermissive")
ENDIF(NOT WIN32)

# Root directory
SET(ROOTDIR ${CMAKE_CURRENT_SOURCE_DIR})

# Python 
FIND_PACKAGE(Python COMPONENTS Interpreter Development)

#################################################################
MACRO(PYTHONTEST name dir)
IF(NOT ${CMAKE_VERSION} VERSION_LESS 2.8.3)
   add_test (NAME PyTest_${name} 
             WORKING_DIRECTORY ${dir}
			 COMMAND ${Python_EXECUTABLE} ${name}.py)
ELSE()
	MESSAGE( "Python Test ${name} skipped! CMake Version > 2.8.3 required." )
ENDIF()
ENDMACRO(PYTHONTEST)
#################################################################

# Tests
OPTION(RUN_TESTS  "Run Tests"  ON)
IF(RUN_TESTS)
ENABLE_TESTING()
ENDIF(RUN_TESTS)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/functions/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/graph/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/printer/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/writer/include)


SET(symbolics_headers   include/Basic.h
                        include/BinaryOp.h
                        include/Zero.h
                        include/Eye.h
                        include/Int.h
                        include/Real.h
                        include/Bool.h
                        include/Error.h
                        include/Factory.h
                        include/Matrix.h
                        include/NaryOp.h 
                        include/Shape.h 
                        include/str.h 
                        include/Symbol.h 
                        include/Symbolics.h
                        include/SymmetricMatrix.h
                        include/UnaryOp.h
                        include/Filesystem.h
                        include/intrusive_ptr.h)

SET(symbolics_sources   Basic.cpp 
                        BinaryOp.cpp
                        Zero.cpp
                        Eye.cpp
                        Int.cpp
                        Real.cpp
                        Bool.cpp
                        Factory.cpp
                        Matrix.cpp 
                        NaryOp.cpp 
                        Shape.cpp 
                        str.cpp 
                        Symbol.cpp 
                        SymmetricMatrix.cpp
                        UnaryOp.cpp
                        Filesystem.cpp)

IF (WIN32)
ELSE()
	ADD_DEFINITIONS(-fPIC)
ENDIF()

ADD_LIBRARY( Symbolics STATIC ${symbolics_sources} ${symbolics_headers} )
TARGET_LINK_LIBRARIES( Symbolics Functions )

ADD_SUBDIRECTORY( functions )
ADD_SUBDIRECTORY( graph )
ADD_SUBDIRECTORY( printer )
ADD_SUBDIRECTORY( test )
ADD_SUBDIRECTORY( wrapper )
ADD_SUBDIRECTORY( writer )

