from distutils.core import setup
import py2exe

setup(
    name='Mcush Debug app',
    version='1.0',
    description='Mcush debug program',
    author='Peng Shulin',
    windows = [
        {
            "script": "McushDebugApp.py",
            "icon_resources": [(1, "img\\logo_inv.ico")],
        }
    ],
    options = {
        "py2exe": {
            "compressed": 1,
            "optimize": 2,
            "dist_dir": "dist-mcush-debug",
        }
    },
)
