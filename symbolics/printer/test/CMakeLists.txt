SET( PRINTER_TEST_sources       main.cpp
								fortran.cpp
								c.cpp
                                matlab.cpp
								matlabsym.cpp
								modelica.cpp
								python.cpp
								FMU.cpp)
                                
ADD_EXECUTABLE( PRINTER_TESTS ${PRINTER_TEST_sources})

TARGET_LINK_LIBRARIES( PRINTER_TESTS Symbolics Functions Printer Graph)
ADD_DEPENDENCIES( PRINTER_TESTS Symbolics Functions Printer Graph)
                                
ADD_TEST (PRINTER_TEST PRINTER_TESTS)
