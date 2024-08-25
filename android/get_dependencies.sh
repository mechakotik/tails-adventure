#!/bin/sh

SDL_VERSION=release-2.30.2
SDL_IMAGE_VERSION=release-2.8.2
SDL_MIXER_VERSION=release-2.8.0
TINYXML2_VERSION=10.0.0

if ! command -v git > /dev/null
then
    echo "Git was not found, please install it"
    exit 1
fi

rm -rf app/jni/SDL
rm -rf app/jni/SDL_image
rm -rf app/jni/SDL_mixer
rm -rf app/jni/tinyxml2

GIT_FLAGS="--depth=1 --recurse-submodules"
git clone https://github.com/libsdl-org/SDL app/jni/SDL -b ${SDL_VERSION} ${GIT_FLAGS}
git clone https://github.com/libsdl-org/SDL_image app/jni/SDL_image -b ${SDL_IMAGE_VERSION} ${GIT_FLAGS}
git clone https://github.com/libsdl-org/SDL_mixer app/jni/SDL_mixer -b ${SDL_MIXER_VERSION} ${GIT_FLAGS}
git clone https://github.com/leethomason/tinyxml2 app/jni/tinyxml2 -b ${TINYXML2_VERSION} ${GIT_FLAGS}

SDL_LAYER_PATH=app/src/main/java/org/libsdl/app
rm -rf ${SDL_LAYER_PATH}
mkdir -p ${SDL_LAYER_PATH}
cp -r app/jni/SDL/android-project/${SDL_LAYER_PATH} ${SDL_LAYER_PATH}/..
