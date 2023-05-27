cmake_minimum_required(VERSION 3.26)
project(TailsAdventure)

set(SDL2IMAGE_VENDORED ON)
set(SDL2MIXER_VENDORED ON)
set(SDL2_DISABLE_INSTALL ON)
set(SDL2IMAGE_INSTALL OFF)
set(SDL2MIXER_INSTALL OFF)

add_subdirectory(app/jni/SDL2)
add_subdirectory(app/jni/SDL2_image)
add_subdirectory(app/jni/SDL2_mixer)

file(GLOB_RECURSE TAILS_ADVENTURE_SRC CONFIGURE_DEPENDS "app/jni/src/*.h" "app/jni/src/*.cpp")
add_executable(TailsAdventure ${TAILS_ADVENTURE_SRC} app/jni/tinyxml2/tinyxml2.cpp)

target_include_directories(TailsAdventure PRIVATE app/jni/src app/jni/tinyxml2)
if(TARGET SDL2::SDL2main)
    target_link_libraries(TailsAdventure PRIVATE SDL2::SDL2main)
endif()
target_link_libraries(TailsAdventure PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer)
target_link_options(TailsAdventure PRIVATE -static-libgcc -static-libstdc++)

set_target_properties(SDL2 SDL2_image SDL2_mixer TailsAdventure
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output
)

add_custom_command(
        TARGET TailsAdventure POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/app/src/main/assets $<TARGET_FILE_DIR:TailsAdventure>/assets
        COMMAND_EXPAND_LISTS
)
