SET( FUNCTIONS_TEST_sources     main.cpp
                                test_abs.cpp
                                test_acos.cpp
                                test_add.cpp
                                test_asin.cpp
                                test_atan.cpp
                                test_atan2.cpp
                                test_cos.cpp
                                test_der.cpp
                                test_element.cpp  
								test_equal.cpp
								test_greater.cpp
								test_if.cpp
								test_less.cpp
                                test_mul.cpp 
                                test_neg.cpp
                                test_pow.cpp
                                test_scalar.cpp
								test_sign.cpp
                                test_sin.cpp
                                test_skew.cpp
                                test_solve.cpp
                                test_tan.cpp
                                test_transpose.cpp
                                test_unknown.cpp
								test_outer.cpp
								test_jacobian.cpp)
                                
SET( FUNCTIONS_TEST_headers     include/test_abs.h
                                include/test_acos.h
                                include/test_add.h
                                include/test_asin.h
                                include/test_atan.h
                                include/test_atan2.h
                                include/test_cos.h
                                include/test_der.h
                                include/test_element.h
								include/test_equal.h
								include/test_greater.h
								include/test_if.h
								include/test_less.h
                                include/test_mul.h 
                                include/test_neg.h
                                include/test_pow.h
                                include/test_scalar.h
								include/test_sign.h
                                include/test_sin.h
                                include/test_skew.h
                                include/test_solve.h
                                include/test_tan.h
                                include/test_transpose.h
                                include/test_unknown.h
								include/test_outer.h
								include/test_jacobian.h)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)                                
                                
ADD_EXECUTABLE( FUNCTIONS_TEST  ${FUNCTIONS_TEST_headers} ${FUNCTIONS_TEST_sources})

TARGET_LINK_LIBRARIES( FUNCTIONS_TEST Symbolics Functions Printer)
ADD_DEPENDENCIES( FUNCTIONS_TEST Symbolics Functions Printer)
                                
ADD_TEST (test_FUNCTIONS FUNCTIONS_TEST)
