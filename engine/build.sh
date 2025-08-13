#!/bin/sh
set echo on

mkdir -p ../bin

cFilenames=$(find . -type f -name "*.c")

echo -e "\e[32mFiles to build in engine:\n$cFilenames\e[0m"

CC=clang
bin="ucima"
CCFlags="-g -shared -fdeclspec -fPIC -O2"
INCLUDEFlags="-I./src"
LDFlags="-lvulkan -lxcb -lX11 -lX11-xcb"
defines="-DUEXPORT"

$CC $cFilenames $CCFlags -o ../bin/lib$bin.so $defines $INCLUDEFlags $LDFlags
