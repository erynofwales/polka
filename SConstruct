# SConstruct
# vim: set ft=python:
#
# Toplevel Scons build script. This should be mostly complete and generic enough
# for most builds.
#
# Eryn Wells <eryn@erynwells.me>

import os

import erw


BUILD_CMDS = get_bool_argument(ARGUMENTS.get('BUILD_CMDS', False))
MODE = ARGUMENTS.get('MODE', None)

BUILD_DIR = Dir('#build')
LIB_DIR = Dir('#lib')
SRC_DIR = Dir('#src')
TEST_DIR = Dir('#test')


def do_sconscript(env, src_dir, out_dir):
    sconscript = src_dir.File('SConscript')
    print 'Reading {}'.format(sconscript)
    # Swapping env and build_env here is a bit wacky. Doing so means that env is
    # always the Environment that the SConscript should be building with, while
    # build_env is the Environment we're using to put everything together.
    env.SConscript(sconscript,
                   {'env': env.Clone(), 'build_env': env},
                   variant_dir=out_dir)

modes = {
    'debug': erw.DebugEnvironment(succinct=not BUILD_CMDS),
    'beta': erw.BetaEnvironment(succinct=not BUILD_CMDS),
    'release': erw.ReleaseEnvironment(succinct=not BUILD_CMDS),
}

for mode in (MODE.split(',') if MODE else ['debug']):
    try:
        env = modes[mode]
    except KeyError:
        print 'Skipping invalid mode: {}'.format(mode)
        break

    out_dir = BUILD_DIR.Dir(env['_name'])

    # Process all lib dirs.
    for lib in os.listdir(LIB_DIR.abspath):
        lib_out_dir = out_dir.Dir('lib').Dir(lib)
        do_sconscript(env, LIB_DIR.Dir(lib), lib_out_dir)
        env.Append(LIBPATH=[lib_out_dir])

    # Get source files.
    src_out_dir = out_dir.Dir('src')
    do_sconscript(env, SRC_DIR, src_out_dir)
    env.Append(LIBPATH=[src_out_dir])

    # Get test binaries.
    do_sconscript(env, TEST_DIR, out_dir.Dir('test'))
