"""
Math and algebra functions that can be used with both numeric and symbolic arguments
"""

import numpy as np
from pymbs import symbolics


def sqrt(arg):
    """
    Calculate square root
    """
    return arg**0.5


def sin(arg):
    """
    Calculate sine
    """
    if isinstance(arg, (int, float)):
        return np.sin(arg)
    else:
        return symbolics.sin(arg)


def asin(arg):
    """
    Calculate arc sine
    """
    if isinstance(arg, (int, float)):
        return np.arcsin(arg)
    else:
        return symbolics.asin(arg)


def cos(arg):
    """
    Calculate cosine
    """
    if isinstance(arg, (int, float)):
        return np.cos(arg)
    else:
        return symbolics.cos(arg)


def acos(arg):
    """
    Calculate arc cosine
    """
    if isinstance(arg, (int, float)):
        return np.arccos(arg)
    else:
        return symbolics.acos(arg)


def tan(arg):
    """
    Calculate tangent
    """
    if isinstance(arg, (int, float)):
        return np.tan(arg)
    else:
        return symbolics.tan(arg)


def atan(arg):
    """
    Calculate arc tangent
    """
    if isinstance(arg, (int, float)):
        return np.arctan(arg)
    else:
        return symbolics.atan(arg)


def atan2(arg1, arg2):
    """
    Calculate arc tangent with two arguments
    """
    if isinstance(arg1, (int, float)) and isinstance(arg2, (int, float)):
        return np.arctan2(arg1, arg2)
    else:
        return symbolics.atan2(arg1, arg2)


def outer(arg1, arg2):
    """
    Outer or dyadic product
    """
    return symbolics.outer(arg1, arg2)


def der(arg):
    """
    Derivative
    """
    if isinstance(arg, (int, float)):
        return 0
    else:
        return symbolics.der(arg)


def diag(vec):
    """
    pymbs.symbolics diagonal matrix
    """
    if isinstance(vec, list):
        n = len(vec)
    elif isinstance(vec, symbolics.Matrix):
        assert len(vec.shape()) == 1
        n = vec.shape()[0]
    else:
        raise ValueError('vec should be a list or a vector (pymbs.symbolics.Matrix) but '
                         f'a {vec.__class__} was encountered')
    matrix = symbolics.Matrix((n, n))
    for i in range(n):
        matrix[i, i] = vec[i]
    return matrix


def norm(vec):
    """
    Calculate norm
    """

    if isinstance(vec, symbolics.zeros):
        return 0

    assert isinstance(vec, symbolics.Basic), f'vec must be a symbolic type not a {vec.__class__}'
    assert vec.shape() in ((3,), (3, 1)), f'vec must be a 3x1 vector but has shape {vec.shape()}'

    n = sqrt(vec[0]**2 + vec[1]**2 + vec[2]**2)
    if isinstance(n, symbolics.Basic):
        n.simplify()

    return n


def symmetric_matrix(seq):
    """
    Creates a symmetric 3x3 matrix from a sequence (list, tuple, Matrix) with 6 elements
    """

    assert isinstance(seq, (list, tuple, symbolics.Matrix))
    matrix = symbolics.Matrix([[seq[0], seq[1], seq[3]],
                              [seq[1], seq[2], seq[4]],
                              [seq[3], seq[4], seq[5]]])

    return matrix


def block_matrix(elements):
    """
    Generates a matrix from a given list of elements, i.e. M = blockMatrix([[A,B], [C,D]])
    """

    # Type check
    assert isinstance(elements, list), \
        f'elements must be a list, not {elements} ({elements.__class__})'
    assert len(elements) > 0, 'element list must not be empty'

    for i in elements:
        assert isinstance(i, list), \
            f'elements in elements must be lists, not {i} ({i.__class__})'
        assert len(i) > 0, 'element list must not be empty'

        for a in i:
            assert isinstance(a, (symbolics.Matrix, symbolics.Variable, symbolics.zeros)), \
                f'only Matrix and Variable are allowed, not {a} ({a.__class__})'
            assert len(a.shape()) == 2, \
                f'elements must be matrices themselves, but {a} has shape {a.shape()}'

    # Calculate shape
    rows = 0   # Rows
    cols = 0   # Columns
    for i in range(len(elements)):
        shape = elements[i][0].shape()
        rows += shape[0]
    for i in range(len(elements[0])):
        shape = elements[0][i].shape()
        cols += shape[1]

    # Set up matrix
    matrix = symbolics.Matrix((rows, cols))

    # Fill it
    m = 0
    for row in elements:
        n = 0
        row_check = (row[0].shape())[0]
        for item in row:
            # Item is the current element
            shape = item.shape()
            assert row_check == shape[0], \
                f'Row size does not match! Expected {row_check} but got {shape[0]}'
            # Copy it
            for i in range(shape[0]):
                for j in range(shape[1]):
                    matrix[m + i, n + j] = item[i, j]
            # Advance to the next column
            n += shape[1]

        # After copying all elements, advance row and check columncount
        assert cols == n, f'Column count does not match! Expected {cols} but got {n}'
        m += row_check

    # Finally check number of rows
    assert rows == m, f'Row count does not match! Expected {rows} but got {m}'

    return matrix


def block_vector(elements):
    """
    Generates a vector from a given list of elements, i.e. v = blockVector([a,b])
    """

    # Type Check
    assert isinstance(elements, list), \
        f'elements must be a list, not {elements} ({elements.__class__})'
    assert len(elements) > 0, 'element list must not be empty'

    for a in elements:
        assert isinstance(a, (symbolics.Matrix, symbolics.Variable, symbolics.zeros)), \
            f'Only Matrix and Variable are allowed, not {a} ({a.__class__})'
        assert len(a.shape()) == 1, \
            f'Elements must be vectors themselves, but {a} has shape {a.shape()}'

    # Calculate Shape
    rows = 0   # Rows
    for i in range(len(elements)):
        shape = elements[i].shape()
        rows += shape[0]

    # Set up Matrix
    v = symbolics.Matrix((rows,))

    # Fill it
    m = 0
    for a in elements:
        # a is the current element
        shape = a.shape()
        # Copy it
        for i in range(shape[0]):
            v[m + i] = a[i]
        m += (a.shape())[0]

    # Finally check number of rows
    assert rows == m, f'Row count does not match! Expected {rows} but got {m}'

    return v


def transpose(arg):
    """
    Transpose of an expression
    """

    if isinstance(arg, (int, float)):
        return arg
    elif isinstance(arg, symbolics.Basic):
        return symbolics.transpose(arg)
    else:
        raise TypeError(f'Type {arg.__class__} not supported in transpose!')


def skew(arg):
    """
    w=skew(v) returns a w, such that w*p = v x p
    """

    if isinstance(arg, symbolics.Matrix):
        v = arg
        assert v.shape() == (3,)

        # We already know what the skew thing is:
        return symbolics.Matrix([[0, -v[2], v[1]],
                                 [v[2], 0, -v[0]],
                                 [-v[1], v[0], 0]])

    if isinstance(arg, symbolics.Basic):
        return symbolics.skew(arg)

    raise TypeError(f'skew is not defined for {arg} ({arg.__class__})')


def skew_numpy(arg):
    """
    w=skew(v) returns a w, such that w*p = v x p
    numpy matrices only
    """
    if isinstance(arg, np.matrix):
        v = arg
        assert v.shape == (3, 1)

        return np.matrix([[0, -v[2], v[1]],
                          [v[2], 0, -v[0]],
                          [-v[1], v[0], 0]])


def scalar_if_possible(arg):
    """
    Try to make the given expression scalar, i.e. if it is a matrix
    """
    if isinstance(arg, symbolics.Matrix):
        if arg.shape() == (1,):
            return arg[0]
        elif arg.shape() == (1, 1):
            return arg[0, 0]
    elif isinstance(arg, symbolics.Basic):
        if (arg.shape() == (1,)) or (arg.shape() == (1, 1)):
            return scalar(arg)

    return arg


def vector_if_possible(arg):
    """
    Try to make the given expression a vector, i.e. if it is a matrix
    """
    if isinstance(arg, np.matrix):
        if arg.shape[1] == 1:
            arg = [el[0] for el in arg.tolist()]

    return arg


def scalar(arg):
    """
    Converts an expression to a scalar
    """

    if isinstance(arg, (int, float)):
        return arg
    else:
        return symbolics.scalar(arg)


def solve(A, b):
    """
    Given Ax=b, one can write x = solve(A,b)
    """
    return symbolics.solve(A, b)


def element(A, row, col):
    """
    Returns a special element from a vector/matrix
    """
    return symbolics.element(A, row, col)


def rot_mat(angle, axis):
    """
    Returns a rotation matrix for a single rotation around the given axis
    """
    # x-Axis
    if axis in (1, 'x', 'X', 'Rx'):
        return symbolics.Matrix([[1, 0, 0],
                                 [0, cos(angle), sin(angle)],
                                 [0, -sin(angle), cos(angle)]])
    # y-Axis
    if axis in (2, 'y', 'Y', 'Ry'):
        return symbolics.Matrix([[cos(angle), 0, -sin(angle)],
                                 [0, 1, 0],
                                 [sin(angle), 0, cos(angle)]])
    # z-Axis
    if axis in (3, 'z', 'Z', 'Rz'):
        return symbolics.Matrix([[cos(angle), sin(angle), 0],
                                 [-sin(angle), cos(angle), 0],
                                 [0, 0, 1]])

    raise ValueError('axis must either be x,y or z')
