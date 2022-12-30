#!/bin/bash

BUILD=build
MEDIA=media
nbr_sheeps=1
nbr_wolfs=1
duration=10

if [ ! -d "$BUILD" ]; then
    mkdir $BUILD
fi

cp -r $MEDIA $BUILD
cd $BUILD
cmake ..
make && ./SDL_project $nbr_sheeps $nbr_wolfs $duration
# chmod +x $auto_test



