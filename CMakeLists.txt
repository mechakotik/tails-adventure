cmake_minimum_required(VERSION 3.25)
project(tails-adventure)

option(TA_STATIC "Statically link libgcc and libstdc++" OFF)
option(TA_UNIX_INSTALL "Install on Unix filesystem" OFF)
option(TA_SANITIZE "Build with sanitizers" OFF)
option(TA_CLANG_TIDY "Run clang-tidy alongside with building" OFF)

if(TA_SANITIZE)
    add_compile_options(-fsanitize=address,leak,undefined)
    add_link_options(-fsanitize=address,leak,undefined)
endif()

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)
set(SDL_STATIC ON)
set(SDL_DISABLE_INSTALL ON)

set(SDLIMAGE_VENDORED ON)
set(SDLIMAGE_DEPS_SHARED OFF)
set(SDLIMAGE_INSTALL OFF)
set(SDLIMAGE_PNG ON)
set(SDLIMAGE_AVIF OFF)
set(SDLIMAGE_BMP OFF)
set(SDLIMAGE_GIF OFF)
set(SDLIMAGE_JPG OFF)
set(SDLIMAGE_JXL OFF)
set(SDLIMAGE_LBM OFF)
set(SDLIMAGE_PCX OFF)
set(SDLIMAGE_PNM OFF)
set(SDLIMAGE_QOI OFF)
set(SDLIMAGE_SVG OFF)
set(SDLIMAGE_TGA OFF)
set(SDLIMAGE_TIF OFF)
set(SDLIMAGE_WEBP OFF)
set(SDLIMAGE_XCF OFF)
set(SDLIMAGE_XPM OFF)
set(SDLIMAGE_XV OFF)

set(SDLMIXER_VENDORED ON)
set(SDLMIXER_DEPS_SHARED OFF)
set(SDLMIXER_INSTALL OFF)
set(SDLMIXER_GME ON)
set(SDLMIXER_FLAC OFF)
set(SDLMIXER_MOD OFF)
set(SDLMIXER_MP3 OFF)
set(SDLMIXER_MIDI OFF)
set(SDLMIXER_OPUS OFF)
set(SDLMIXER_WAVE OFF)
set(SDLMIXER_WAVPACK OFF)

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

if(TA_CLANG_TIDY)
    find_program(CLANG_TIDY NAMES "clang-tidy")
    set_target_properties(tails-adventure PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY}")
endif()

if(TARGET SDL2::SDL2main)
    target_link_libraries(tails-adventure PRIVATE SDL3::SDL3main)
endif()

target_link_libraries(tails-adventure PRIVATE
    SDL3::SDL3-static
    SDL3_image::SDL3_image-static
    SDL3_mixer::SDL3_mixer-static
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

if(TA_UNIX_INSTALL)
    target_compile_options(tails-adventure PRIVATE -DTA_UNIX_INSTALL)
    install(TARGETS tails-adventure DESTINATION /usr/bin)
    install(DIRECTORY assets/ DESTINATION /usr/share/tails-adventure)
    install(FILES external/SDL_GameControllerDB/gamecontrollerdb.txt DESTINATION /usr/share/tails-adventure)
    install(FILES res/tails-adventure.desktop DESTINATION /usr/share/applications)
    install(FILES res/tails-adventure.png DESTINATION /usr/share/icons)
else()
    install(TARGETS tails-adventure DESTINATION ${CMAKE_BINARY_DIR}/output)
    install(DIRECTORY assets DESTINATION ${CMAKE_BINARY_DIR}/output)
    install(FILES external/SDL_GameControllerDB/gamecontrollerdb.txt DESTINATION ${CMAKE_BINARY_DIR}/output/assets)
endif()
