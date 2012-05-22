#!/bin/bash
LD_LIBRARY_PATH=/opt/agava/lib gdb --args /usr/bin/python -c 'from agava import gost3411; print gost3411("aaa")'


