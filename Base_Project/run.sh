#!/bin/bash

BUILD=build
MEDIA=media
PRO=Project_SDL_Part1_base
if [ ! -d "$BUILD" ]; then
    mkdir $BUILD
fi

cd $BUILD
cp -r $MEDIA $BUILD
cmake ..
make 
sleep 2
# cd $BUILD
# if [[ -f "$auto_test" && ! -f "$DIR/$auto_test" ]]; then
#     cp $auto_test $DIR
# fi
cd $PRO

./SDL_part1 1 2 3
# chmod +x $auto_test



