#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from setuptools import setup, Extension

modulel = Extension(
	'sandbox',
	include_dirs=['.'],
	libraries=['seccomp', 'pthread'],
	sources=['src/rules/clike.cpp', 'src/child.cpp', 'src/exception.cpp',
		'src/parent.cpp', 'src/runner.cpp', 'python_module.cpp']
)

setup(
	name='sandbox',
	version='1.1.0',
	description='Run application in sandbox',
	author='Monad',
	author_email='YanWQmonad@gmail.com',
	ext_modules=[modulel]
)
