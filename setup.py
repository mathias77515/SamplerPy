from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import setuptools
import pybind11

class get_pybind_include(object):
    """Helper class to determine the pybind11 include path"""
    def __str__(self):
        return pybind11.get_include()

ext_modules = [
    Extension(
        'SamplerPy',                              # name of the module
        ['src/MH.cpp', 'src/utils.cpp'],  # your source files
        include_dirs=[
            get_pybind_include(),
            # add other include dirs, e.g. current directory, or where UF23Field.h lives
            '.',
        ],
        language='c++',
        extra_compile_args=['-std=c++17'],  # match your C++ standard
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