project('itc', 'c',
  version : '0.1',
  license: ['LGPL'],
  license_files: ['LICENSE'],
  meson_version: '>=1.4.0',
  subproject_dir: 'vendor',
  default_options : [
        'prefix=' + meson.current_build_dir(),
        'c_std=gnu99',
        'default_library=both',
        'warning_level=3',
    ]
)

# Common compiler args used with for all libs and executables
common_c_args = meson.get_compiler('c').get_supported_arguments([
    '-Wbidi-chars=any,ucn',
    '-Wcast-align',
    '-Wcast-qual',
    '-Wdangling-else',
    '-Wdeclaration-after-statement',
    '-Wdouble-promotion',
    '-Wduplicated-cond',
    '-Wduplicated-branches',
    '-Wformat=2',
    '-Wformat-nonliteral',
    '-Wformat-security',
    '-Wimplicit-fallthrough=3',
    '-Winit-self',
    '-Wjump-misses-init',
    '-Wlogical-op',
    '-Wmissing-prototypes',
    '-Wmultistatement-macros',
    '-Wnested-externs',
    '-Wnull-dereference',
    '-Wold-style-definition',
    '-Wpointer-arith',
    '-Wshadow',
    '-Wshift-overflow=2',
    # '-Wstack-usage=8192',
    '-Wstrict-prototypes',
    '-Wswitch-default',
    '-Wundef',
    '-Wwrite-strings',
])

# Common link args used for all libs and executables
common_link_args = meson.get_compiler('c').get_supported_link_arguments([
])

# Build LibITC
subdir('libitc')
subdir('libitc' / 'config')

# Build unit tests
if get_option('tests')
    cmock_proj = subproject('cmock')
    cmock_dep = cmock_proj.get_variable('cmock_dep')
    unity_dep = cmock_proj.get_variable('unity_dep')

    ruby = find_program('ruby', required: true)
    valgrind = find_program('valgrind', required: false)
    unity_test_runner_generator_script = meson.current_source_dir() / 'vendor' / 'cmock' / 'vendor' / 'unity' / 'auto' / 'generate_test_runner.rb'

    libitc_test_src_dir = 'libitc' / 'test'
    subdir(libitc_test_src_dir)
    subdir(libitc_test_src_dir / 'config')
endif