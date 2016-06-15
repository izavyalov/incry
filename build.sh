#!/bin/bash

# g++ --version
# Configured with: --prefix=/Applications/Xcode.app/Contents/Developer/usr --with-gxx-include-dir=/usr/include/c++/4.2.1
# Apple LLVM version 7.3.0 (clang-703.0.31)
# Target: x86_64-apple-darwin15.4.0
# Thread model: posix
# InstalledDir: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin

g++ -std=c++11 main.cpp  -g -O3 -o incry

