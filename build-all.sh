#!/bin/bash
set echo on

echo "Building..."

pushd engine
source build-debug.sh
popd

ERRNO=$?
if [ $ERRNO -ne 0 ]; then
  echo "Exit code: "$ERRNO && exit
fi

pushd testapp
source build.sh
popd

ERRNO=$?
if [ $ERRNO -ne 0 ]; then
  echo "Exit code: "$ERRNO && exit
fi

echo -e "See result in the \e[34mbin\e[0m folder if it exist"
