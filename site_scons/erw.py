# erw.py
# Eryn Wells <eryn@erynwells.me>

import sys

import SCons.Environment

import paths

#
# Environments
#

class Environment(SCons.Environment.Environment):
    '''
    Default SCons environment for building things.
    '''

    def __init__(self, name, modern=True, paranoid=True, colorful=True, succinct=True, **kwargs):
        super(Environment, self).__init__(**kwargs)

        self['NAME'] = name

        if 'CC' not in kwargs:
            self['CC'] = self.Detect(['clang', 'gcc'])
        if 'CXX' not in kwargs:
            self['CXX'] = self.Detect(['clang++', 'g++'])
        if 'LINK' not in kwargs:
            self['LINK'] = self.Detect(['clang++', 'clang', 'ld'])

        # Modern C/C++
        if modern:
            self.Append(CFLAGS=['-std=c99'])
            self.Append(CXXFLAGS=['-std=c++11'])

        # Paranoid C/C++
        if paranoid:
            self.Append(CCFLAGS=['-Wall', '-Wextra', '-pedantic'])

        # Colorful C/C++
        if colorful and sys.stdout.isatty():
            if 'clang' in self['CC'] or 'clang' in self['CXX']:
                self.Append(CCFLAGS=['-fcolor-diagnostics'])

        self['ARCOMSTR'] = self._comstr('Archiving', succinct)
        self['ASCOMSTR'] = self._comstr('Assembling', succinct)
        self['ASPPCOMSTR'] = self._comstr('Assembling', succinct)
        self['CCCOMSTR'] = self._comstr('Building (C)', succinct)
        self['CXXCOMSTR'] = self._comstr('Building (C++)', succinct)
        self['LINKCOMSTR'] = self._comstr('Linking', succinct)
        self['RANLIBCOMSTR'] = self._comstr('Indexing', succinct)
        self['SHCCCOMSTR'] = self._comstr('Building (C, Shared)', succinct)
        self['SHCXXCOMSTR'] = self._comstr('Building (C++, Shared)', succinct)
        self['SHLINKCOMSTR'] = self._comstr('Linking (Shared)', succinct)

    def _comstr(self, action, succinct=True):
        if succinct:
            return '{:>25}: $TARGET'.format(action)
        else:
            return '  [{:^6}] $TARGET'.format(action)


class DebugEnvironment(Environment):
    def __init__(self, name='debug', **kwargs):
        super(DebugEnvironment, self).__init__(name, **kwargs)
        self.Append(CPPDEFINES=['DEBUG'])
        self.Append(CCFLAGS=['-O0', '-g'])


class BetaEnvironment(Environment):
    def __init__(self, name='beta', **kwargs):
        super(BetaEnvironment, self).__init__(name, **kwargs)
        self.Append(CPPDEFINES=['DEBUG'])
        self.Append(CCFLAGS=['-O3', '-g'])


class ReleaseEnvironment(Environment):
    def __init__(self, name='release', **kwargs):
        super(ReleaseEnvironment, self).__init__(name, **kwargs)
        self.Append(CPPDEFINES=['NDEBUG', 'RELEASE'])
        self.Append(CCFLAGS=['-O3'])
