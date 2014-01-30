# SConstruct
# vim: set ft=python:
#
# Toplevel Scons build script for the Charles project.
#
# Eryn Wells <eryn@erynwells.me>


#
# DEFAULT CONFIGURATION VALUES
#

# Enabling debugging does the following things:
#   1. Turns on debugging symbols
#   2. Turns off all optimization
#   3. Sets the DEBUG define
DEBUG = True

# Show build commands ("cc [args] -o [out] [file], etc"). If this is False, show
# some nice messages for each step of the build.
BUILD_CMDS = False

# Source directories. New directories should contain a SConscript file and be
# added here.
source_directories = Split("""
    #src
""")

#
# BUILD STUFF BELOW HERE
#

import os.path

cflags='-Wall -Wextra -pedantic'
common_env = Environment(
    CC='clang',
    CXX='clang++',
    CFLAGS=cflags + ' -std=c99',
    CXXFLAGS=cflags + ' -std=c++11')

def get_bool_argument(arg):
    if bool(int(arg)):
        return True
    if arg == ('True', 'TRUE', 'true'):
        return True
    return False

BUILD_CMDS = get_bool_argument(ARGUMENTS.get('BUILD_CMDS', BUILD_CMDS))
if not BUILD_CMDS:
    def generate_comstr(action):
        return '%18s: $TARGET' % (action,)
    common_env['ASCOMSTR'] = generate_comstr('Assembling'),
    common_env['CCCOMSTR'] = generate_comstr('Building (C)'),
    common_env['CXXCOMSTR'] = generate_comstr('Building (C++)'),
    common_env['LINKCOMSTR'] = generate_comstr('Linking'),
    common_env['ARCOMSTR'] = generate_comstr('Archiving'),
    common_env['RANLIBCOMSTR'] = generate_comstr('Indexing')

debug_env = common_env.Clone()
debug_env.VariantDir(os.path.join('build', 'debug'), 'src', duplicate=0)
debug_env.Append(CPPDEFINES=['DEBUG'])
debug_cflags = ' -O0 -g'
debug_env.Append(CFLAGS=debug_cflags, CXXFLAGS=debug_cflags)

release_env = common_env.Clone()
release_env.VariantDir(os.path.join('build', 'release'), 'src', duplicate=0)
release_cflags = ' -O2'
release_env.Append(CPPDEFINES=['RELEASE'])

for d in source_directories:
    sc = os.path.join(d, 'SConscript')
    debug_env.SConscript(sc, {'env': debug_env})
    release_env.SConscript(sc, {'env': release_env})
