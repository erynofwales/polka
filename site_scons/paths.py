# paths.py
# Eryn Wells <eryn@erynwells.me>

import os
import os.path


def is_executable(path):
    return os.path.exists(path) and os.access(path, os.X_OK)


def which(program):
    '''
    Look for `program` in system path and return the full path to that binary if
    it is found. Otherwise, return `None`.
    '''
    path, name = os.path.split(program)
    if path:
        if is_executable(program):
            return program
    else:
        pathext = [''] + os.environ.get('PATHEXT', '').split(os.pathsep)
        for path in os.environ.get('PATH', '').split(os.pathsep):
            exe = os.path.join(path, program)
            for ext in pathext:
                candidate = exe + ext
                if is_executable(candidate):
                    return candidate
    return None

