# sconscript.py
# Eryn Wells <eryn@erynwells.me>

'''
SCons tool for working with SConscripts.
'''

import SCons.Script

def _do_sconscript(env):
    original_sconscript = env.SConscript

    def sconscript(env, sconscript, clone=False, *args, **kwargs):
        exports = {
            'Library': env.Library,
            'Object': env.Object,
            'SharedObject': env.SharedObject,
            'StaticLibrary': env.StaticLibrary,
            'SharedLibrary': env.SharedLibrary,
            'Program': env.Program,
            'env': env.Clone() if clone else env,
        }
        SCons.Script._SConscript.GlobalDict.update(exports)
        env.log('Reading {}'.format(sconscript))
        return original_sconscript(sconscript, {}, *args, **kwargs)

    return sconscript

#
# SCons tool interface
#

def generate(env):
    env.AddMethod(_do_sconscript(env), 'SConscript')


def exists(env):
    return True
