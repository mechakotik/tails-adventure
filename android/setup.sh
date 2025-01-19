#!/bin/sh

TEMPLATE_PATH=../external/SDL/android-project
SDL_LAYER_PATH=app/src/main/java/org/libsdl/app

rm -r ${SDL_LAYER_PATH}
mkdir -p ${SDL_LAYER_PATH}
cp -r ${TEMPLATE_PATH}/${SDL_LAYER_PATH} ${SDL_LAYER_PATH}/..

rm -r gradle
rm gradlew
rm gradlew.bat
mkdir gradle
cp -r ${TEMPLATE_PATH}/gradle gradle/..
cp ${TEMPLATE_PATH}/gradlew .
cp ${TEMPLATE_PATH}/gradlew.bat .

rm ../assets/gamecontrollerdb.txt
cp ../external/SDL_GameControllerDB/gamecontrollerdb.txt ../assets/
