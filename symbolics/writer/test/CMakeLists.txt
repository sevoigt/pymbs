##################################################################
# Test
MACRO(TEST name args)
IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.6)

   SET(testname ${name}_Test)

   ADD_EXECUTABLE( ${name} ${args} pendulum.h)
   TARGET_LINK_LIBRARIES( ${name} Symbolics Functions Printer Writer Graph)
   ADD_DEPENDENCIES( ${name} Symbolics Functions Printer Writer Graph)
    
    # Test hinzufuegen
    add_test (${testname} ${name})
ENDIF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.6)    
ENDMACRO(TEST)


#################################################################

TEST(WRITER_TESTS writers.cpp)



