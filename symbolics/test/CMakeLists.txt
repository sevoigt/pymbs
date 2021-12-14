##################################################################
# Test
MACRO(TEST name args)
IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.6)

   SET(testname ${name}_Test)

   ADD_EXECUTABLE( ${name} ${args})
   TARGET_LINK_LIBRARIES( ${name} Symbolics Functions Printer)
   ADD_DEPENDENCIES( ${name} Symbolics Functions Printer)
    
    # Test hinzufuegen
    add_test (${testname} ${name})
ENDIF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.6)    
ENDMACRO(TEST)

#################################################################

SET(symbolic_test_sources main.cpp )
TEST(Symbolic_Test ${symbolic_test_sources} )

TEST(SHAPE shape.cpp)
TEST(SYMBOL symbol.cpp)

TEST(CONSTANT constant.cpp)
TEST(MATRIX matrix.cpp)
TEST(SIMPLIFY simplify.cpp)
TEST(UTIL util.cpp)
TEST(SUBS subs.cpp)
TEST(SYMMETRICMATRIX symmetricmatrix.cpp)


ADD_EXECUTABLE( complexity complexity.cpp)
TARGET_LINK_LIBRARIES( complexity Symbolics Functions Printer)
ADD_DEPENDENCIES( complexity Symbolics Functions Printer)
