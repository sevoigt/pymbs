class PyMbsError(Exception):
    '''
    PyMbs' own exception type.
    '''

    def __init__(self, message):
        self.msg = message
        Exception.__init__(self, self.msg)
