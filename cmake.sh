#!/bin/bash
mkdir build -p
pushd build > /dev/null
cmake -Wno-dev -G "$1" ..
popd > /dev/null
