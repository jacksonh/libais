#!/usr/bin/env python

from distutils.core import setup, Extension

ais_module = Extension('ais',
                    sources = ['ais_py.cpp',
                               'ais.cpp',
                               'ais1_2_3.cpp',
                               'ais4_11.cpp',
                               'ais5.cpp',
                               # ais6
                               'ais7_13.cpp',
                               'ais8.cpp',
                               'ais8_001_22.cpp',
                               'ais8_366_22.cpp',
                               'ais9.cpp',
                               'ais10.cpp',
                               'ais12.cpp',
                               'ais14.cpp',
                               'ais15.cpp',
                               'ais18.cpp',
                               'ais19.cpp',
                               'ais21.cpp',
                               'ais24.cpp',
                               ])

setup (name = 'ais',
       version = '0.4',
       description = ' Automatic Identification System',
       ext_modules = [ais_module])
