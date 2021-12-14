##################################################################
# Test
MACRO(TEST name args)
IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.6)

   SET(testname ${name}_Test)

   ADD_EXECUTABLE( ${testname} ${args})
   TARGET_LINK_LIBRARIES( ${testname} Graph)
   ADD_DEPENDENCIES( ${testname} Graph)
    
    # Test hinzufuegen
    add_test (${testname} ${testname})
ENDIF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.6)    
ENDMACRO(TEST)
#################################################################

TEST(GRAPH graph.cpp)
PYTHONTEST(CraneCrab ${ROOTDIR}/../Examples)
TEST(EQUATIONSYSTEM equationsystem.cpp)
