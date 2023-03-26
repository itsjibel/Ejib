#!/bin/bash
g++ src/ejib.cpp -o ejib -lX11 -O3 -Ofast -Og -Os -s -pipe -march=native -funroll-all-loops -flto=auto --std=c++23