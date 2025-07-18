from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import pybind11
import subprocess
import sysconfig

class get_pybind_include(object):
    """Helper class to determine the pybind11 include path"""
    def __str__(self):
        return pybind11.get_include()

def brew_prefix(package_name):
    try:
        prefix = subprocess.check_output(["brew", "--prefix", package_name], text=True).strip()
        return prefix
    except subprocess.CalledProcessError:
        return None

#libomp_prefix = brew_prefix("libomp")
#python_include = sysconfig.get_paths()["include"]

ext_modules = [
    Extension(
        'SamplerPy',                              # name of the module
        ['src/MH.cpp', 'src/utils.cpp'],  # your source files
        include_dirs=[
            #python_include,
            # add other include dirs, e.g. current directory, or where UF23Field.h lives
            '.',
        ],
        language='c++',
        extra_compile_args=[
            "-Xpreprocessor", "-fopenmp",
            "-I/opt/homebrew/include"  # or /usr/local/include
        ],
        extra_link_args=[
            "-lomp",
            "-L/opt/homebrew/lib"  # or /usr/local/lib
        ],
    ),
]


setup(
    name='SamplerPy',
    version='1.0.0',
    author='Regnier Mathias',
    description='',
    ext_modules=ext_modules,
    cmdclass={'build_ext': build_ext},
    zip_safe=False,
    install_requires=[],
)