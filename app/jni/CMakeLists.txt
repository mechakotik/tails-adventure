cmake_minimum_required(VERSION 3.26)
project(TailsAdventure)

set(SDL2_DISABLE_INSTALL OFF)
set(SDL2_IMAGE_INSTALL OFF)
set(SDL2_MIXER_INSTALL OFF)

add_subdirectory(SDL2)
add_subdirectory(SDL2_image)
add_subdirectory(SDL2_mixer)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

file(GLOB_RECURSE TAILS_ADVENTURE_SRC CONFIGURE_DEPENDS "src/*.h" "src/*.cpp")
add_executable(TailsAdventure ${TAILS_ADVENTURE_SRC} tinyxml2/tinyxml2.cpp)

include_directories(src tinyxml2)

if(TARGET SDL2main)
    target_link_libraries(TailsAdventure SDL2main)
endif()
target_link_libraries(TailsAdventure SDL2 SDL2_image SDL2_mixer)

target_link_options(TailsAdventure PRIVATE -static-libgcc -static-libstdc++)

add_custom_command(
        TARGET TailsAdventure POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:TailsAdventure> $<TARGET_FILE_DIR:TailsAdventure>
        COMMAND_EXPAND_LISTS
)

add_custom_command(
        TARGET TailsAdventure POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/../src/main/assets $<TARGET_FILE_DIR:TailsAdventure>/assets
        COMMAND_EXPAND_LISTS
)
