
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source Files
SET( Graph_headers include/Equation.h
                   include/Graph.h 
                   include/Node.h 
                   include/Assignments.h
				   include/SystemHandler.h
                   include/EquationSystem.h
				   include/MatchedSystem.h
				   include/UnMatchedSystem.h
				   include/PreOptimisation.h
				   include/PastOptimisation.h)
SET( Graph_sources Equation.cpp
                   Graph.cpp 
                   Node.cpp 
                   Assignments.cpp
                   EquationSystem.cpp
				   MatchedSystem.cpp
				   UnMatchedSystem.cpp
				   PreOptimisation.cpp
				   PastOptimisation.cpp)

# Target
ADD_LIBRARY( Graph STATIC ${Graph_headers} ${Graph_sources} )
ADD_DEPENDENCIES( Graph Symbolics Functions )
TARGET_LINK_LIBRARIES( Graph Symbolics Functions Printer)

ADD_SUBDIRECTORY( Test )