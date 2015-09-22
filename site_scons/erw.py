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
        # Use clang if its available.
        kwargs.setdefault('CC', self._toolchain_binary(('clang', 'gcc')))
        kwargs.setdefault('CXX', self._toolchain_binary(('clang++', 'g++')))
        kwargs.setdefault('LINK', self._toolchain_binary(('clang++')))

        super(Environment, self).__init__(**kwargs)

        self['_name'] = name

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

    def _toolchain_binary(self, binaries):
        for b in binaries:
            if b and paths.which(b):
                return b

    def _comstr(self, action, succinct=True):
        if succinct:
            return '{:>25}: $TARGET'.format(action)
        else:
            return '  [{:^6}] $TARGET'.format(action)


class DebugEnvironment(Environment):
    def __init__(self, name='debug', **kwargs):
        super(DebugEnvironment, self).__init__(name, **kwargs)
        self.Append(CPPDEFINES=['NDEBUG'])
        self.Append(CCFLAGS=['-O0', '-g'])


class BetaEnvironment(Environment):
    def __init__(self, name='beta', **kwargs):
        super(BetaEnvironment, self).__init__(name, **kwargs)
        self.Append(CPPDEFINES=['NDEBUG'])
        self.Append(CCFLAGS=['-O3', '-g'])


class ReleaseEnvironment(Environment):
    def __init__(self, name='release', **kwargs):
        super(ReleaseEnvironment, self).__init__(name, **kwargs)
        self.Append(CPPDEFINES=['NRELEASE'])
        self.Append(CCFLAGS=['-O3'])
