# swiftc.py
# vim: set ft=python:
# Eryn Wells <eryn@erynwells.me>

'''
SCons plugin for building Swift files with swiftc.
'''

import SCons.Action
import SCons.Tool
import SCons.Util

SwiftSuffix = '.swift'
SwiftAction = SCons.Action.Action("$SWIFTCCOM", "$SWIFTCCOMSTR")

compilers = ['swiftc']

def generate(env):
    static_obj, shared_obj = SCons.Tool.createObjBuilders(env)
    static_obj.add_action(SwiftSuffix, SwiftAction)
    static_obj.add_emitter(SwiftSuffix, SCons.Defaults.SharedObjectEmitter)
    shared_obj.add_action(SwiftSuffix, SwiftAction)
    shared_obj.add_emitter(SwiftSuffix, SCons.Defaults.SharedObjectEmitter)

    if 'SWIFTC' not in env:
        compiler = env.Detect(compilers)
        env['SWIFTC'] = compiler if compiler else compilers[0]
    env['SWIFTFLAGS'] = SCons.Util.CLVar('')
    env['SWIFTCCOM']   = '$SWIFTC -o $TARGET -c $SWIFTFLAGS $SOURCES'
    env['SWIFTFILESUFFIX'] = SwiftSuffix

def exists(env):
    return env.Detect(compilers)
