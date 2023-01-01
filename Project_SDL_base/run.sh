#!/bin/bash

BUILD=build
MEDIA=media
nbr_sheeps=2
nbr_wolfs=2
duration=10

if [ $# -gt 0 ]; then
    nbr_sheeps=$1
    if [ $# -gt 1 ]; then
        nbr_wolfs=$2
        if [ $# -gt 2 ]; then
            duration=$3
        fi
    fi
fi


if [ ! -d "$BUILD" ]; then
    mkdir $BUILD
fi

cp -r $MEDIA $BUILD
cd $BUILD
cmake ..
make && ./SDL_project $nbr_sheeps $nbr_wolfs $duration
# chmod +x $auto_test



