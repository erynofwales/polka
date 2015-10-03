# osxapp.py
# Eryn Wells <eryn@erynwells.me>

import os.path

_BUNDLE_SUFFIX = '.app'


def _assemble_app_bundle(env, bundle_name, binary, info_plist, resources_dir):
    # TODO: Create directory structure, copy binary, plist, and resources. Sign the binary? app bundle?

    if not bundle_name.endswith('.app'):
        bundle_name += '.app'

    bundle_dir = env.Dir(bundle_name)
    contents_dir = bundle_dir.Dir('Contents')
    macos_dir = contents_dir.Dir('MacOS')
    resources_dir = contents_dir.Dir('Resources')

    return [env.Mkdir(contents_dir),
            env.Copy(contents_dir.File('Info.plist'), info_list),
            env.Mkdir(macos_dir),
            env.Copy(macos_dir.File(bundle_name), binary)


_app_builder = Builder(action=_assemble_app_bundle,
                       suffix=_BUNDLE_SUFFIX)


def generate(env):
    env['OSXAPPBUNDLESUFFIX'] = _BUNDLE_SUFFIX
    env['BUILDERS']['OSXApp'] = _app_builder


def exists(env):
    return True
