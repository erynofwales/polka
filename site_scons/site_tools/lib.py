# lib.py
# Eryn Wells <eryn@erynwells.me>

'''
SCons builder for a lib directory.
'''

import os

import SCons.Errors
import SCons.Script


def _lib(env, name):
    return env['LIBS'].get(name)


def _register_lib(env, name, lib):
    if name in env['LIBS']:
        env.log_error('Library has already been built: {}'.format(name))
    env['LIBS'][name] = lib


def _lib_dirs(env):
    for lib in os.listdir(env.lib_root.abspath):
        lib_dir = env.lib_root.Dir(lib)
        if not lib_dir.isdir():
            continue
        yield (lib, lib_dir)


def _process_lib_dirs(env):
    for name, _ in _lib_dirs(env):
        env.LibDir(name)


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
    out = env.SConscript(src_dir.File('SConscript'),
                         clone=True,
                         variant_dir=out_dir)
    return out


def _build_library(env, lib_func):
    original_builder = lib_func

    def builder(env, lib_name, sources, *args, **kwargs):
        lib = original_builder(lib_name, sources, *args, **kwargs)
        _register_lib(env, lib_name, lib)
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
    env.AddMethod(_lib, 'lib')
    env.AddMethod(_process_lib_dirs, 'process_lib_dirs')

def exists(env):
    return True
