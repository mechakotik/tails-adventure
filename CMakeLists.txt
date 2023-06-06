cmake_minimum_required(VERSION 3.26)
project(TailsAdventure)

set(SDL2IMAGE_VENDORED ON)
set(SDL2MIXER_VENDORED ON)
set(SDL2_DISABLE_INSTALL ON)
set(SDL2IMAGE_INSTALL OFF)
set(SDL2MIXER_INSTALL OFF)

add_subdirectory(external/SDL)
add_subdirectory(external/SDL_image)
add_subdirectory(external/SDL_mixer)

file(GLOB_RECURSE TAILS_ADVENTURE_SRC CONFIGURE_DEPENDS "include/*.h" "src/*.cpp")
set(TAILS_ADVENTURE_SRC ${TAILS_ADVENTURE_SRC} external/tinyxml2/tinyxml2.cpp)
add_executable(TailsAdventure ${TAILS_ADVENTURE_SRC})

target_include_directories(TailsAdventure PRIVATE include include/objects external/tinyxml2)
if(TARGET SDL2::SDL2main)
    target_link_libraries(TailsAdventure PRIVATE SDL2::SDL2main)
endif()
target_link_libraries(TailsAdventure PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer)
target_link_options(TailsAdventure PRIVATE -static -static-libgcc -static-libstdc++)

set_target_properties(TailsAdventure SDL2 SDL2_image SDL2_mixer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output
)
if(NOT WIN32)
    set_target_properties(SDL2 SDL2_image SDL2_mixer PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib
    )
endif()

add_custom_command(
        TARGET TailsAdventure POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:TailsAdventure>/assets
        COMMAND_EXPAND_LISTS
)
