libitc_lib = library(
    meson.project_name(),
    libitc_src,
    include_directories: [libitc_inc, libitc_pkg_inc],
    version: meson.project_version(),
    c_args: libitc_c_args + common_c_args,
    link_args: libitc_link_args + common_link_args,
    install: true,
    install_dir: libitc_install_dir
)

libitc_dep = declare_dependency(
    include_directories: libitc_inc,
    link_with: libitc_lib,
)

libitc_test_dep = declare_dependency(
    include_directories: [libitc_inc, libitc_pkg_inc],
    link_with: libitc_lib,
)
