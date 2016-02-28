# site_init.py
# Eryn Wells <eryn@erynwells.me>

import logging
import sys
import SCons.Environment
import SCons.Errors


def setup_logging(level=logging.DEBUG):
    '''Configure global logging for the SCons system.'''
    root = logging.getLogger()
    root.setLevel(logging.DEBUG)
    root.addHandler(logging.StreamHandler())

#
# Argument utils
#

def get_bool_argument(arg):
    '''
    Convert the given argument value to a bool. True values are any integer that
    is considered true by Python, and any string value that isn't a
    capitalization variant of the word "false".
    '''
    try:
        return bool(int(arg))
    except ValueError:
        pass
    return str(arg).lower() != 'false'

#
# Builders
#

def program_builder(env):
    original_builder = env.Program
    def builder(env, prog_name, sources, *args, **kwargs):
        return original_builder(prog_name, sources, *args, **kwargs)
    return builder

#
# Environments
#

class Environment(SCons.Environment.Environment):
    '''
    Default SCons environment for building things.
    '''

    @classmethod
    def _comstr(cls, action, succinct=True):
        if succinct:
            return '{:>25}: $TARGET'.format(action)
        # Use the default *COMSTR.
        return None

    @classmethod
    def for_mode(cls, mode, name=None, succinct=True):
        kwargs = {'succinct': succinct}
        if name:
            kwargs['name'] = name
        mode_lower = mode.lower()
        if mode_lower == 'debug':
            return DebugEnvironment(**kwargs)
        elif mode_lower == 'beta':
            return BetaEnvironment(**kwargs)
        elif mode_lower == 'release':
            return ReleaseEnvironment(**kwargs)
        raise SCons.Errors.UserError('Invalid mode: {}'.format(mode))

    def __init__(self, name, modern=True, paranoid=True, colorful=True, succinct=True, **kwargs):
        super(Environment, self).__init__(**self._append_custom_tools(kwargs))

        self.SetDefault(NAME=name)

        self.SetDefault(LOGGER=logging.getLogger(name))

        self.SetDefault(BUILD_ROOT=self.Dir('#build'))
        self.SetDefault(LIB_ROOT=self.Dir('#lib'))
        self.SetDefault(SRC_ROOT=self.Dir('#src'))
        self.SetDefault(TEST_ROOT=self.Dir('#test'))
        self.SetDefault(INCLUDE_ROOT=self.Dir('#include'))

        # Allow same directory includes.
        self.AppendUnique(CPPPATH=['.'])

        self['CC'] = self.Detect(['clang', 'gcc'])
        self['CXX'] = self.Detect(['clang++', 'g++'])
        self['LINK'] = self.Detect(['clang++', 'clang', 'ld'])

        # Modern C/C++
        if modern:
            self.AppendUnique(CFLAGS=['-std=c99'])
            self.AppendUnique(CXXFLAGS=['-std=c++11'])

        # Paranoid C/C++
        if paranoid:
            self.AppendUnique(CCFLAGS=['-Wall', '-Wextra', '-pedantic'])

        # Colorful C/C++
        if colorful and sys.stdout.isatty():
            if 'clang' in self['CC'] or 'clang' in self['CXX']:
                self.AppendUnique(CCFLAGS=['-fcolor-diagnostics'])
            elif 'gcc' in self['CC'] or 'g++' in self['CXX']:
                # TODO: Also set a GCC_COLORS variable in the system environment?
                self.AppendUnique(CCFLAGS=['-fdiagnostics-color=always'])

        # Pretty printing
        self.SetDefault(ARCOMSTR=Environment._comstr('Archiving', succinct))
        self.SetDefault(ASCOMSTR=Environment._comstr('Assembling', succinct))
        self.SetDefault(ASPPCOMSTR=Environment._comstr('Assembling', succinct))
        self.SetDefault(CCCOMSTR=Environment._comstr('Building (C)', succinct))
        self.SetDefault(CXXCOMSTR=Environment._comstr('Building (C++)', succinct))
        self.SetDefault(LINKCOMSTR=Environment._comstr('Linking', succinct))
        self.SetDefault(RANLIBCOMSTR=Environment._comstr('Indexing', succinct))
        self.SetDefault(SHCCCOMSTR=Environment._comstr('Building (C, Shared)', succinct))
        self.SetDefault(SHCXXCOMSTR=Environment._comstr('Building (C++, Shared)', succinct))
        self.SetDefault(SHLINKCOMSTR=Environment._comstr('Linking (Shared)', succinct))

    @property
    def build_root(self):
        '''Return the build output directory for this environment.'''
        return self['BUILD_ROOT'].Dir(self['NAME'])

    @property
    def lib_root(self):
        '''Return the root directory for libraries for this environment.'''
        return self['LIB_ROOT']

    @property
    def src_root(self):
        '''Return the main source root directory for this environment.'''
        return self['SRC_ROOT']

    def process_src(self):
        out_dir = self.build_root.Dir('src')
        self.SConscript(self.src_root.File('SConscript'),
                        variant_dir=out_dir)
        self.Append(CPPPATH=[self.src_root])

    #
    # Test processing
    #

    def test_objects(self, name):
        self._ensure_test_structure(name)
        return self['TESTS'][name]['objects']

    def test_program(self, name):
        self._ensure_test_structure(name)
        return self['TESTS'][name]['program']

    def register_test_program(self, name, prog):
        self._ensure_test_structure(name)
        self['TESTS'][name]['program'] = prog

    def process_tests(self):
        tests = []
        for test, struct in self['TESTS'].iteritems():
            if not struct['program']:
                continue
            tests.append(self.TestRun(test))
        self.Alias('test', tests)

    def _ensure_test_structure(self, name):
        self['TESTS'].setdefault(name, {'program': None, 'objects': []})

    #
    # Logging
    #

    def log(self, msg, *args, **kwargs):
        self['LOGGER'].info(msg, *args, **kwargs)

    def log_debug(self, msg, *args, **kwargs):
        self['LOGGER'].debug(msg, *args, **kwargs)

    def log_error(self, msg, *args, **kwargs):
        self['LOGGER'].error(msg, *args, **kwargs)

    def _append_custom_tools(self, kwargs):
        '''Add custom tools to the `kwargs`.'''
        tools = kwargs.setdefault('tools', ['default'])
        for tool in ['lib', 'test', 'program', 'sconscript', 'swiftc']:
            if tool in tools:
                continue
            tools.append(tool)
        return kwargs


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
