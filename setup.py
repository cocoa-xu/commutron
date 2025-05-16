#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from setuptools import setup, Extension

setup(
    ext_modules=[
        Extension(
            name="commutron.core",
            sources=["src/commutron.cpp"],
            extra_compile_args=["-O3", "-std=c++17", "-fPIC"],
        ),
    ],
)
