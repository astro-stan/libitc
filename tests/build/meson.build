fs = import('fs')

libitc_normal_test_bin_install_dir = libitc_test_bin_install_dir / 'normal'
libitc_mocked_test_bin_install_dir = libitc_test_bin_install_dir / 'mocked'

# Entrypoint for the unit tests
# Expects Unity to generate the unit test runner function
libitc_test_entrypoint_file = '''/* The test runner function */
extern int testRunner(void);

/* Test entrypoint */
int main(void)
{
    return testRunner();
}
'''
libitc_test_entrypoint_c = 'main.c'
libitc_test_config_yaml_path = meson.current_source_dir() / 'config.yaml'

# Add valgrind as a custom test setup
if valgrind.found()
    add_test_setup(
        'valgrind',
        exe_wrapper: [valgrind, '--leak-check=full', '--error-exitcode=1', '--track-origins=yes', '-s'],
        is_default: true,
    )
endif

# Create the entrypoint source file for all tests
libitc_test_entrypoint_c_tgt = custom_target(
    'Unit test entrypoint',
    build_always_stale: true,
    build_by_default: false,
    command: [
        ruby,
        '-e' ,
        'File.write(\'' + '@OUTDIR@' / libitc_test_entrypoint_c + '\', \'' + libitc_test_entrypoint_file + '\')',
    ],
    output: [libitc_test_entrypoint_c],
)

libitc_test_exes_common_arg = [
    libitc_test_entrypoint_c_tgt,
    libitc_test_common_src,
]

libitc_test_exes_common_kwargs = {
    'include_directories': [
        libitc_test_inc
    ],
    'dependencies': [
        libitc_test_dep,
        unity_dep,
        cmock_dep,
    ],
    'c_args': libitc_test_c_args + common_c_args,
    'link_args': libitc_test_link_args + common_link_args,
}

subdir('normal')
subdir('mocked')
