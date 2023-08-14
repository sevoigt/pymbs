
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/include )

# Source Files
SET(Functions_headers   include/Abs.h
                        include/Acos.h
                        include/Add.h
                        include/Asin.h
                        include/Atan.h
                        include/Atan2.h
                        include/Cos.h
                        include/Der.h
                        include/Element.h
                        include/Mul.h 
                        include/Neg.h
                        include/Operators.h
                        include/Pow.h
                        include/Scalar.h
                        include/Sin.h
                        include/Skew.h
					            	include/Outer.h
                        include/Solve.h
                        include/Tan.h
                        include/Transpose.h
                        include/Unknown.h
                        include/Util.h
                        include/List.h
                        include/If.h
                        include/Greater.h
                        include/Less.h
                        include/Equal.h
                        include/Sign.h
                        include/Jacobian.h
                        include/Inverse.h)

SET(Functions_sources   Abs.cpp
                        Acos.cpp
                        Add.cpp
                        Asin.cpp
                        Atan.cpp
                        Atan2.cpp
                        Cos.cpp
                        Der.cpp
                        Element.cpp
                        Mul.cpp 
                        Neg.cpp
                        Operators.cpp
                        Pow.cpp
                        Scalar.cpp
                        Sin.cpp
                        Skew.cpp
            						Outer.cpp
                        Solve.cpp
                        Tan.cpp
                        Transpose.cpp
                        Unknown.cpp
                        Util.cpp
                        List.cpp
                        If.cpp
                        Greater.cpp
                        Less.cpp
                        Equal.cpp
                        Sign.cpp
                        Jacobian.cpp
                        Inverse.cpp)

# Plattform-Libs
IF(WIN32)
  SET(platform_libs)
ELSEIF(APPLE)
  SET(platform_libs)
ELSE()
  SET(platform_libs rt)
ENDIF(WIN32)

# Target
IF(WIN32)
ELSE()
	ADD_DEFINITIONS(-fPIC)
ENDIF()

ADD_LIBRARY( Functions STATIC ${Functions_headers} ${Functions_sources} )
ADD_DEPENDENCIES( Functions Symbolics )
TARGET_LINK_LIBRARIES( Functions Symbolics ${platform_libs})

ADD_SUBDIRECTORY(test)

