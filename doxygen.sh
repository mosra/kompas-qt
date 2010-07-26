#!/bin/bash

mkdir -p build/doc

# Link in Core documentation
cp ../core/build/doc/map2x-core.tag build/doc/map2x-core.tag

# Generate Qt tag file, if not present
[ -f build/doc/qt.tag ] || doxytag -t build/doc/qt.tag /usr/share/doc/qt/html/

doxygen
