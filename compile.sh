#!/bin/sh
mkdir build
cd build
cmake -G "Unix Makefiles" .. -DCMAKE_BUILD_TYPE=Debug
cmake --build ./ --target app --config Debug
