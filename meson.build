project(
    'ta-remake', 'cpp',
    default_options: 'cpp_std=c++20'
)

sdl2_dep = dependency('sdl2')
sdl2_image_dep = dependency('sdl2_image')
sdl2_mixer_dep = dependency('sdl2_mixer')
tinyxml2_dep = dependency('tinyxml2')
deps = [sdl2_dep, sdl2_image_dep, sdl2_mixer_dep, tinyxml2_dep]

sources = files()
subdir('src')

if host_machine.system() == 'windows'
    windows = import('windows')
    sources += windows.compile_resources('res/icon.rc')
endif

incdir = include_directories('include', 'include/objects')
output_dir = meson.build_root() + '/output'

link_args = []
if host_machine.system() == 'windows' and get_option('prefer_static')
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
