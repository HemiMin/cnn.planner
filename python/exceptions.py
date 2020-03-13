r"""
Collections of Exceptions.
"""

class ValueException(Exception):
    r"""
    Exception which is raised when the value of data or module is wrong.
    """

    def __str__(self):
        return 'ValueExecption: The value of data or module is wrong.'

class FileNotExistException(Exception):
    r"""
    Exception which is raised when the file is not exist.
    """

    def __init__(self, file_name):
        super(FileNotExistException, self).__init__(file_name)
        self.file_name = file_name

    def __str__(self):
        return 'FileNotExistException: {} is not exist.'.format(self.file_name)

class NotSupportedException(Exception):
    r"""
    Exception when unsupported layer is in.
    """

    def __init__(self, module):
        super(NotSupportedException, self).__init__(module)
        self.module = module

    def __str__(self):
        return 'Layer type: {} is not supported.'.format(str(self.module))

class RuntimeFail(Exception):
    r"""
    Exception which is raised during planner runtime.
    """

class CodeGenerationFail(RuntimeFail):
    r"""
    RuntimeFail when code generation is failed.
    """

    def __str__(self):
        return 'CodeGenerationFail: Simulation code generation is failed.'

class CodeCompileFail(RuntimeFail):
    r"""
    Simulation code compiler is failed.
    """

    def __str__(self):
        return 'CodeCompilerFail: Simulation code compiler is failed.'

class RunCodeFail(RuntimeFail):
    r"""
    Running simulation code is failed.
    """

    def __str__(self):
        return 'RunCodeFail: Running simulation code is failed.'

class ReportFail(RuntimeFail):
    r"""
    Reporting is failed.
    """

    def __str__(self):
        return 'ReportFail: Reporting is failed.'
