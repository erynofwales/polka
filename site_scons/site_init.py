# site_init.py
# Eryn Wells <eryn@erynwells.me>

import erw

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
