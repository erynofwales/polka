# test.py
# Eryn Wells <eryn@erynwells.me>

'''
Test builder for SCons. Test files are compiled to objects and stored in the 
environment.
'''

def _process_test_dir(env, dir, program=None):
    # TODO: Builder for test directories?
    pass


def _build_test_object(env, source, program=None):
    obj = env.Object(source)
    if not program:
        program = 'test'
    try:
        env.test_objects(program).extend(obj)
    except TypeError:
        env.test_objects(program).append(obj)
    return obj


def _build_test_program(env, name=None):
    if not name:
        name = 'test'
    prog = env.Program(name, env.test_objects(name), LIBS=[env.lib('gtest')])
    env.register_test_program(name, prog)
    return 


def _run_tests(env, name=None):
    if not name:
        name = 'test'
    cmd = env.Command(env.test_program(name), None, '$SOURCE --gtest_color=yes')
    env.AlwaysBuild(cmd)
    return cmd

#
# SCons tool interface
#

def generate(env):
    env.SetDefault(TESTS={})
    env.AddMethod(_build_test_object, 'Test')
    env.AddMethod(_build_test_program, 'TestProgram')
    env.AddMethod(_run_tests, 'TestRun')

def exists(env):
    return 'Object' in env
