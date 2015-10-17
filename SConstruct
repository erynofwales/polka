# SConstruct
# vim: set ft=python:
#
# Toplevel Scons build script. This should be mostly complete and generic enough
# for most builds.
#
# Eryn Wells <eryn@erynwells.me>

import logging

setup_logging()

BUILD_CMDS = get_bool_argument(ARGUMENTS.get('BUILD_CMDS', False))
MODE = ARGUMENTS.get('MODE', None)

for mode in (MODE.split(',') if MODE else ['debug']):
    env = Environment.for_mode(mode)
    env.process_lib_dirs()
    env.process_src()
    env.process_tests()
