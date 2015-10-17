# lib.py
# Eryn Wells <eryn@erynwells.me>

'''
SCons builder for a lib directory.
'''

import SCons.Errors
import SCons.Script


def _process_lib_dir(env, lib, src_dir=None, out_dir=None, inc_dir=None):
    if not src_dir:
        src_dir = env.lib_root.Dir(lib)
    if not src_dir.isdir():
        err = 'Invalid library source directory: {}'.format(src_dir)
        env.log_error(err)
        raise SCons.Errors.UserError(err)
    if not out_dir:
        out_dir = env.build_root.Dir('lib').Dir(lib)
    if not inc_dir:
        include_dir = src_dir.Dir('include')
        if include_dir.isdir():
            inc_dir = [include_dir]
    env.Append(CPPPATH=inc_dir)
    exports = {'Library': env.Library,
               'StaticLibrary': env.StaticLibrary,
               'SharedLibrary': env.SharedLibrary}
    SCons.Script._SConscript.GlobalDict.update(exports)
    out = env.SConscript(src_dir.File('SConscript'),
                         {'env': env.Clone()},
                         variant_dir=out_dir,
                         exports=exports)
    return out


def _build_library(env, lib_func):
    original_builder = lib_func

    def builder(env, lib_name, sources, *args, **kwargs):
        lib = original_builder(lib_name, sources, *args, **kwargs)
        env.register_lib(lib_name, lib)
        return lib

    return builder

#
# SCons tool interface
#

def generate(env):
    env.SetDefault(LIBS={})
    env.AddMethod(_process_lib_dir, 'LibDir')
    env.AddMethod(_build_library(env, env.Library), 'Library')
    env.AddMethod(_build_library(env, env.StaticLibrary), 'StaticLibrary')
    env.AddMethod(_build_library(env, env.SharedLibrary), 'SharedLibrary')

def exists(env):
    return True
