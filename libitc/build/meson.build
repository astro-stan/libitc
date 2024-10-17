libitc_lib = library(
    meson.project_name(),
    libitc_src,
    include_directories: [libitc_inc, libitc_pkg_inc],
    version: meson.project_version(),
    c_args: meson.get_compiler('c').get_supported_arguments([
        common_c_args,
        libitc_c_args,
    ]),
    link_args: meson.get_compiler('c').get_supported_link_arguments([
        common_link_args,
        libitc_link_args,
    ]),
    install: true,
    override_options: [
        # Allow undefined symbols to exist
        # Used when compiling with static memory allocation support
        'b_lundef=false'
    ]
)

libitc_dep = declare_dependency(
    include_directories: libitc_inc,
    link_with: libitc_lib,
)

libitc_test_dep = declare_dependency(
    include_directories: [libitc_inc, libitc_pkg_inc],
    link_with: libitc_lib,
)
