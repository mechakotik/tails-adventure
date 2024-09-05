cmake_minimum_required(VERSION 3.25)
project(tails-adventure)

option(TA_STATIC OFF)
option(TA_LINUX_INSTALL OFF)

set(CMAKE_CXX_STANDART 20)
set(BUILD_SHARED_LIBS OFF)
set(SDL_STATIC ON)
set(SDL2_DISABLE_INSTALL ON)

set(SDL2IMAGE_VENDORED ON)
set(SDL2IMAGE_DEPS_SHARED OFF)
set(SDL2IMAGE_INSTALL OFF)
set(SDL2IMAGE_PNG ON)
set(SDL2IMAGE_AVIF OFF)
set(SDL2IMAGE_BMP OFF)
set(SDL2IMAGE_GIF OFF)
set(SDL2IMAGE_JPG OFF)
set(SDL2IMAGE_JXL OFF)
set(SDL2IMAGE_LBM OFF)
set(SDL2IMAGE_PCX OFF)
set(SDL2IMAGE_PNM OFF)
set(SDL2IMAGE_QOI OFF)
set(SDL2IMAGE_SVG OFF)
set(SDL2IMAGE_TGA OFF)
set(SDL2IMAGE_TIF OFF)
set(SDL2IMAGE_WEBP OFF)
set(SDL2IMAGE_XCF OFF)
set(SDL2IMAGE_XPM OFF)
set(SDL2IMAGE_XV OFF)

set(SDL2MIXER_VENDORED ON)
set(SDL2MIXER_DEPS_SHARED OFF)
set(SDL2MIXER_INSTALL OFF)
set(SDL2MIXER_GME ON)
set(SDL2MIXER_FLAC OFF)
set(SDL2MIXER_MOD OFF)
set(SDL2MIXER_MP3 OFF)
set(SDL2MIXER_MIDI OFF)
set(SDL2MIXER_OPUS OFF)
set(SDL2MIXER_WAVE OFF)
set(SDL2MIXER_WAVPACK OFF)

set(GME_ZLIB ON)
set(USE_GME_VGM ON)
set(USE_GME_AY OFF)
set(USE_GME_GBS OFF)
set(USE_GME_GYM OFF)
set(USE_GME_HES OFF)
set(USE_GME_KSS OFF)
set(USE_GME_NSF OFF)
set(USE_GME_NSFE OFF)
set(USE_GME_SAP OFF)
set(USE_GME_SPC OFF)

add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_mixer EXCLUDE_FROM_ALL)

if(WIN32)
    enable_language("RC")
    set(WIN32_RESOURCES res/icon.rc)
endif()

file(STRINGS "src/sources.list" TA_SOURCES)
add_executable(tails-adventure
    ${TA_SOURCES}
    external/tinyxml2/tinyxml2.cpp
    ${WIN32_RESOURCES}
)

if(TARGET SDL2::SDL2main)
    target_link_libraries(tails-adventure PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(tails-adventure PRIVATE
    SDL2::SDL2-static
    SDL2_image::SDL2_image-static
    SDL2_mixer::SDL2_mixer-static
)
target_include_directories(tails-adventure PRIVATE
    include
    include/objects
    external/SDL/include
    external/SDL_image/include
    external/SDL_mixer/include
    external/tinyxml2
)

if(TA_STATIC)
    if(WIN32)
        target_link_options(tails-adventure PRIVATE -static)
    else()
        target_link_options(tails-adventure PRIVATE -static-libgcc -static-libstdc++)
    endif()
endif()

if(WIN32)
    target_compile_options(tails-adventure PRIVATE -mwindows)
    target_link_options(tails-adventure PRIVATE -mwindows)
endif()

if(TA_LINUX_INSTALL)
    target_compile_options(tails-adventure PRIVATE -DTA_LINUX_INSTALL)
    install(TARGETS tails-adventure DESTINATION /usr/bin)
    install(DIRECTORY assets/ DESTINATION /usr/share/tails-adventure)
    install(FILES res/tails-adventure.desktop DESTINATION /usr/share/applications)
    install(FILES res/tails-adventure.png DESTINATION /usr/share/icons)
else()
    install(TARGETS tails-adventure DESTINATION ${CMAKE_BINARY_DIR}/output)
    install(DIRECTORY assets DESTINATION ${CMAKE_BINARY_DIR}/output)
endif()
