# site_init.py
# Eryn Wells <eryn@erynwells.me>


def do_sconscript(env, src_dir, out_dir=None):
    '''
    Look for a SConscript file in `src_dir` and run it. Return any result.
    '''
    sconscript = src_dir.File('SConscript')
    print 'Reading {}'.format(sconscript)
    kwargs = {}
    if out_dir:
        kwargs['variant_dir'] = out_dir
    return env.SConscript(sconscript, {'env': env}, **kwargs)

#
# Argument utils
#

def get_bool_argument(arg):
    '''
    Convert the given argument value to a bool. True values are any integer that
    is considered true by Python, and any string value that isn't a
    capitalization variant of the word "false".
    '''
    try:
        return bool(int(arg))
    except ValueError:
        pass
    return str(arg).lower() != 'false'
