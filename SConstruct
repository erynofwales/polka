# SConstruct
# vim: set ft=python:
#
# Toplevel Scons build script. This should be mostly complete and generic enough
# for most builds.
#
# Eryn Wells <eryn@erynwells.me>


#
# DEFAULT CONFIGURATION VALUES
#

# Settings for the default toolchain binaries. Setting these overrides the
# defaults, provided your the given binary exists.
CC = None
CXX = None
AS = None
LINK = None

# Same as above but for default CFLAGS. These are appended to both CFLAGS and
# CXXFLAGS.
CFLAGS = '-Wall -Wextra -pedantic'

# Enabling debugging does the following things:
#   1. Turns on debugging symbols
#   2. Turns off all optimization
#   3. Sets the DEBUG define
DEBUG = True

# Show build commands ("cc [args] -o [out] [file]", etc). If this is False, show
# some nice messages for each step of the build.
BUILD_CMDS = False

#
# BUILD STUFF BELOW HERE
#

import os
import os.path


def which(program):
    def is_executable(path):
        return os.path.exists(path) and os.access(path, os.X_OK)

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


def get_bool_argument(arg):
    try:
        return bool(int(arg))
    except ValueError:
        pass
    if arg in ('False', 'FALSE', 'false', ''):
        return False
    return True


def set_compiler(env, var, user_compiler, compilers, compiler_string=None):
    if not string:
        string = 'compiler'
    if user_compiler:
        if which(user_compiler):
            env[var] = user_compiler
        else:
            raise SCons.Errors.UserError('The given {} does not exist. '
                                         ':-('.format(compiler_string))
    else:
        for c in compilers:
            if which(c):
                env[var] = c
                break
        else:
            raise SCons.Errors.UserError(("Couldn't find a viable {}. "
                                          "Have you installed one?").format(
                                              compiler_string))

common_env = Environment()
set_compiler(common_env, 'CC', CC, ('clang', 'gcc'), 'C compiler')
set_compiler(common_env, 'CXX', CXX, ('clang++', 'g++'), 'C++ compiler')
set_compiler(common_env, 'AS', AS, ('gas'), 'assembler')
set_compiler(common_env, 'LINK', LINK, ('ld'), 'linker')
common_env.Append(CFLAGS='{} -std=c99'.format(CFLAGS))
common_env.Append(CXXFLAGS='{} -std=c++11'.format(CFLAGS))

BUILD_CMDS = get_bool_argument(ARGUMENTS.get('BUILD_CMDS', BUILD_CMDS))
if not BUILD_CMDS:
    def generate_comstr(action):
        return '{:>18}: $TARGET'.format(action)
    common_env['ASCOMSTR'] = generate_comstr('Assembling')
    common_env['CCCOMSTR'] = generate_comstr('Building (C)')
    common_env['CXXCOMSTR'] = generate_comstr('Building (C++)')
    common_env['LINKCOMSTR'] = generate_comstr('Linking')
    common_env['ARCOMSTR'] = generate_comstr('Archiving')
    common_env['RANLIBCOMSTR'] = generate_comstr('Indexing')

src_dir = Dir('#src')

debug_env = common_env.Clone()
debug_env.VariantDir(os.path.join('build', 'debug'), src_dir, duplicate=0)
debug_env.Append(CPPDEFINES=['DEBUG'])
debug_cflags = ' -O0 -g'
debug_env.Append(CFLAGS=debug_cflags, CXXFLAGS=debug_cflags)

release_env = common_env.Clone()
release_env.VariantDir(os.path.join('build', 'release'), src_dir, duplicate=0)
release_env.Append(CPPDEFINES=['RELEASE'])
release_cflags = ' -O2'
release_env.Append(CFLAGS=release_cflags, CXXFLAGS=release_cflags)

for mode, env in {'debug': debug_env, 'release': release_env}.iteritems():
    env.SConscript(os.path.join('build', mode, 'SConscript'), {'env': env})
