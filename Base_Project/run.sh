#!/bin/bash

BUILD=build
MEDIA=media
PRO=Project_SDL_Part1_base
if [ ! -d "$BUILD" ]; then
    mkdir $BUILD
fi

cp -r $MEDIA $BUILD
cd $BUILD
cmake ..
make && cd $PRO

./SDL_part1 2 2 10
# chmod +x $auto_test



