#!/bin/sh
set echo on

mkdir -p ../bin

cFilenames=$(find . -type f -name "*.c")

echo -e "\e[32mFiles to build in testapp:\n$cFilenames\e[0m"

CC=clang
bin="testapp"
CCFlags="-Wall"
INCLUDEFlags="-I./src -I../engine/src/ -I/usr/include"
LDFlags="-L../bin/ -lucima -Wl,-rpath,."
defines="-D_DEBUG -DDCMIMPORT"

$CC $cFilenames $CCFlags -o ../bin/$bin $defines $INCLUDEFlags $LDFlags
