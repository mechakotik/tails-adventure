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
linux_install = get_option('linux_install')

if(linux_install)
    exec_name = 'tails-adventure'
    exec_dir = '/usr/bin'
    assets_dir = '/usr/share/tails-adventure'
    cpp_args = ['-DTA_LINUX_INSTALL']
else
    exec_name = 'TailsAdventure'
    exec_dir = meson.build_root() + '/output'
    assets_dir = meson.build_root() + '/output/assets'
    cpp_args = []
endif

executable(
    exec_name, sources,
    win_subsystem: 'windows',
    dependencies: deps,
    cpp_args: cpp_args,
    include_directories: incdir,
    install: true,
    install_dir: exec_dir
)

install_subdir('assets', install_dir: assets_dir, strip_directory: true)
if(linux_install)
    install_data('res/tails_adventure.desktop', install_dir: '/usr/share/applications')
    install_data('res/tails-adventure.png', install_dir: '/usr/share/icons')
endif
