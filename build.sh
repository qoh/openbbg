#!/bin/bash
pushd build > /dev/null
cmake --build . --config RelWithDebInfo
popd > /dev/null
