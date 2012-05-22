from distutils.core import setup, Extension
from os import uname
setup(
    name='agava',
    version='0.1',
    ext_modules=[Extension('agava', ['main.c'], extra_link_args=['-O0', '-ggdb', '-L/opt/agava/lib/','agava/linux2.6.x-i386-%d/libagava.a' % (uname()[4] == 'x86_64' and 64 or 32)], extra_compile_args=['-lagava','-O0', '-ggdb', '-I/opt/agava/include/'])]
)

