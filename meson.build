project(
    'ta-remake', 'cpp',
    default_options: 'cpp_std=c++20'
)

linux_install = get_option('linux_install')
static = get_option('static')

sdl2_dep = dependency('sdl2', static: static)
sdl2_image_dep = dependency('sdl2_image', static: static)
sdl2_mixer_dep = dependency('sdl2_mixer', static: static)
tinyxml2_dep = dependency('tinyxml2', static: static)
deps = [sdl2_dep, sdl2_image_dep, sdl2_mixer_dep, tinyxml2_dep]

fs = import('fs')
sources_list = fs.read('src/sources.list').strip().split('\n')
sources = files(sources_list)

if host_machine.system() == 'windows'
    windows = import('windows')
    sources += windows.compile_resources('res/icon.rc')
endif

incdir = include_directories('include', 'include/objects')

if(linux_install)
    exec_dir = '/usr/bin'
    assets_dir = '/usr/share/tails-adventure'
    cpp_args = ['-DTA_LINUX_INSTALL']
else
    exec_dir = meson.build_root() + '/output'
    assets_dir = meson.build_root() + '/output/assets'
    cpp_args = []
endif

link_args = []
if(static)
	link_args = ['-static']
endif

executable(
    'tails-adventure', sources,
    win_subsystem: 'windows',
    dependencies: deps,
    cpp_args: cpp_args,
	link_args: link_args,
    include_directories: incdir,
    install: true,
    install_dir: exec_dir
)

install_subdir('assets', install_dir: assets_dir, strip_directory: true)
if(linux_install)
    install_data('res/tails-adventure.desktop', install_dir: '/usr/share/applications')
    install_data('res/tails-adventure.png', install_dir: '/usr/share/icons')
endif
