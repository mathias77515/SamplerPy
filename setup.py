from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import pybind11
import subprocess
import sysconfig
import os

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

llvm_path = '/opt/homebrew/opt/llvm'
sdk_path = os.popen('xcrun --show-sdk-path').read().strip()

extra_compile_args = [
    '-O2',
    '-Xpreprocessor', '-fopenmp',
    f'-I{llvm_path}/include',
    '-arch', 'arm64',
    '-isysroot', sdk_path,
]

extra_link_args = [
    f'-L{llvm_path}/lib',
    '-lomp',
    '-arch', 'arm64',
    '-isysroot', sdk_path,
    f'-Wl,-rpath,{llvm_path}/lib',
]

ext_modules = [
    Extension(
        'SamplerPy',
        sources=['src/MH.cpp', 'src/utils.cpp'],
        include_dirs=[sysconfig.get_paths()['include'], f'{llvm_path}/include'],
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
        language='c++',
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