from .graph import Graph, VarKind

from .symbolics import CMatrix as SymmetricMatrix, \
                       CMatrix as Matrix, \
                       CNumber as Number, \
                       CZeros as zeros, \
                       CEye as eye, \
                       CBasic as Basic, \
                       CSymbol as Symbol, \
                       CSymbol as Variable

from .symbolics import acos, asin, atan, atan2, sin, cos, tan, \
                       element, scalar, skew, der, solve, \
                       transpose, outer, jacobian, inv, \
                       If, Less, Greater, Equal