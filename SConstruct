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


#
# BUILD STUFF BELOW HERE
#

import os
import os.path
import SCons.Errors


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


def set_toolchain_binary(env, var, user_binary, binaries=()):
    if user_binary and which(user_binary):
        env[var] = user_binary
        return
    for c in binaries:
        if which(c):
            env[var] = c
            break

common_env = Environment()
set_toolchain_binary(common_env, 'CC', CC, ('clang', 'gcc'))
set_toolchain_binary(common_env, 'CXX', CXX, ('clang++', 'g++'))
set_toolchain_binary(common_env, 'AS', AS)
set_toolchain_binary(common_env, 'LINK', LINK)
common_env.Append(CFLAGS='{} -std=c99'.format(CFLAGS))
common_env.Append(CXXFLAGS='{} -std=c++11'.format(CFLAGS))

# Add color error messages for clang
if 'clang' in common_env['CC']:
    common_env.Append(CFLAGS=' -fcolor-diagnostics')
if 'clang' in common_env['CXX']:
    common_env.Append(CXXFLAGS=' -fcolor-diagnostics')

BUILD_CMDS = get_bool_argument(ARGUMENTS.get('BUILD_CMDS', False))
if not BUILD_CMDS:
    def generate_comstr(action):
        return '{:>25}: $TARGET'.format(action)
    common_env['ARCOMSTR'] = generate_comstr('Archiving')
    common_env['ASCOMSTR'] = generate_comstr('Assembling')
    common_env['ASPPCOMSTR'] = generate_comstr('Assembling')
    common_env['CCCOMSTR'] = generate_comstr('Building (C)')
    common_env['CXXCOMSTR'] = generate_comstr('Building (C++)')
    common_env['LINKCOMSTR'] = generate_comstr('Linking')
    common_env['RANLIBCOMSTR'] = generate_comstr('Indexing')
    common_env['SHCCCOMSTR'] = generate_comstr('Building (C, Shared)')
    common_env['SHCXXCOMSTR'] = generate_comstr('Building (C++, Shared)')
    common_env['SHLINKCOMSTR'] = generate_comstr('Linking (Shared)')

build_dir = Dir('#build')
lib_dir = Dir('#lib')
src_dir = Dir('#src')
test_dir = Dir('#test')


def create_env(name, src_dirs, appends=None):
    output_dir = build_dir.Dir(name)
    env = common_env.Clone()
    env['__name'] = name
    env['__build_dir'] = output_dir
    env['__src_dirs'] = []
    env['__output_dirs'] = []
    for d in src_dirs:
        out_dir = output_dir.Dir(d.path)
        env['__src_dirs'].append(d)
        env['__output_dirs'].append(out_dir)
        env.VariantDir(out_dir, d.path, duplicate=0)
        env.Clean('.', out_dir)
    if appends:
        for k, v in appends.iteritems():
            if k.startswith('='):
                env[k[1:]] = v
            else:
                env.Append(**{k: v})
    return env


debug_cflags = ' -O0 -g'
debug_env = create_env('debug', [src_dir], {
    'CPPDEFINES': ['DEBUG'],
    'CFLAGS': debug_cflags,
    'CXXFLAGS': debug_cflags,
})

release_cflags = ' -O2'
release_env = create_env('release', [src_dir], {
    'CPPDEFINES': ['RELEASE'],
    'CFLAGS': release_cflags,
    'CXXFLAGS': release_cflags,
})

modes = {
    'debug': debug_env,
    'release': release_env,
}

mode = ARGUMENTS.get('MODE', None)
build_modes = []
if mode:
    # If MODE=foo is specified, build only that mode.
    build_modes.append(mode)
else:
    build_modes = modes.keys()

for mode in build_modes:
    try:
        env = modes[mode]
    except KeyError:
        print 'Skipping invalid mode: {}'.format(mode)
    for d in env['__output_dirs']:
        env.SConscript(d.File('SConscript'), {'env': env})
