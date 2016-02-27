# vim: set ft=python:
#
# Toplevel Scons build script. This should be mostly complete and generic enough
# for most builds.
#
# Eryn Wells <eryn@erynwells.me>

import logging

setup_logging()

MODE = ARGUMENTS.get('MODE', None)
SUCCINCT = bool(ARGUMENTS.get('SUCCINCT', True))

for mode in (MODE.split(',') if MODE else ['debug']):
    env = Environment.for_mode(mode, succinct=SUCCINCT)
    env.process_src()
