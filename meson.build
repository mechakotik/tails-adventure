project(
    'ta-remake', 'cpp',
    default_options: 'cpp_std=c++20'
)

static = not get_option('shared')

sdl2_dep = dependency('sdl2', static: static)
sdl2_image_dep = dependency('sdl2_image', static: static)
sdl2_mixer_dep = dependency('sdl2_mixer', static: static)
tinyxml2_dep = dependency('tinyxml2', static: static)
deps = [sdl2_dep, sdl2_image_dep, sdl2_mixer_dep, tinyxml2_dep]

sources = files()
subdir('src')

windows = import('windows')
sources += windows.compile_resources('res/icon.rc')

incdir = include_directories('include', 'include/objects')
output_dir = meson.build_root() + '/output'

link_args = []
if static
    link_args = ['-static']
endif

executable(
    'TailsAdventure', sources,
    win_subsystem: 'windows',
    dependencies: deps,
    include_directories: incdir,
    link_args: link_args,
    install: true,
    install_dir: output_dir
)

install_subdir('assets', install_dir: output_dir)
