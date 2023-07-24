project(
    'ta-remake', 'cpp',
    default_options: 'cpp_std=c++20'
)

subproject('updater')

sdl2_dep = dependency('sdl2', static: true)
sdl2_image_dep = dependency('sdl2_image', static: true)
sdl2_mixer_dep = dependency('sdl2_mixer', static: true)
tinyxml2_dep = dependency('tinyxml2', static: true)
deps = [sdl2_dep, sdl2_image_dep, sdl2_mixer_dep, tinyxml2_dep]

sources = files()
subdir('src')

incdir = include_directories('include', 'include/objects')
output_dir = meson.build_root() + '/output'

executable(
    'TailsAdventure', sources,
    win_subsystem: 'windows',
    dependencies: deps,
    include_directories: incdir,
    link_args: ['-static'],
    install: true,
    install_dir: output_dir
)

install_subdir('assets', install_dir: output_dir)
install_data('version', install_dir: output_dir)
