# program.py
# Eryn Wells <eryn@erynwells.me>

'''
SCons tool for working with Programs.
'''

def _program(env, name):
    return env['PROGRAMS'].get(name)


def _register_program(env, name, program):
    env['PROGRAMS'][name] = program


def _build_program(env):
    original_builder = env.Program

    def builder(env, name, sources, local_libs=None, *args, **kwargs):
        # local_libs is an array of names of libs built in the local project.
        # These will be looked up in the environment and added to the LIBS
        # array, if present.
#        if local_libs:
#            local_libs = map(lambda lib: env.lib(lib), local_libs)
#            try:
#                kwargs['LIBS'].extend(local_libs)
#            except KeyError:
#                kwargs['LIBS'] = local_libs
        prog = original_builder(name, sources, *args, **kwargs)
        _register_program(env, name, prog)
        return prog

    return builder

#
# SCons tool interface
#

def generate(env):
    env.SetDefault(PROGRAMS={})
    env.AddMethod(_build_program(env), 'Program')
    env.AddMethod(_program, 'program')


def exists(env):
    return True
