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
libitc_test_bin_install_dir = 'bin' / 'tests'
libitc_test_config_yaml = 'config.yaml'

fs = import('fs')

# Add valgrind as a custom test setup
if valgrind.found()
    add_test_setup(
        'valgrind',
        exe_wrapper: [valgrind, '--leak-check=full', '--error-exitcode=1'],
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

# Iterate over all unit test source files
foreach libitc_test_src_file_path : libitc_test_src
    libitc_test_src_file = fs.name(libitc_test_src_file_path.full_path())
    libitc_test_src_name = fs.replace_suffix(libitc_test_src_file, '')

    libitc_test_runner_c = libitc_test_src_name + '_Runner.c'
    libitc_test_runner_h = libitc_test_src_name + '.h'

    # Generate the test runner via the unity runner generator script
    libitc_test_runner_ch_tgt = custom_target(
        libitc_test_src_file + ' runner',
        build_by_default: false,
        command: [
            ruby,
            unity_test_runner_generator_script,
            libitc_test_src_file_path.full_path(),
            '@OUTDIR@' / libitc_test_runner_c,
            '@CURRENT_SOURCE_DIR@' / libitc_test_config_yaml,
            '--header_file=@OUTDIR@' / libitc_test_runner_h,
            '--test_prefix=@BASENAME@_',
            '--main_name=testRunner',
        ],
        input: [libitc_test_src_file_path],
        output: [libitc_test_runner_c, libitc_test_runner_h],
    )

    # Generate the test executable
    libitc_test_exe = executable(
        libitc_test_src_name.replace('_', ''),
        libitc_test_src_file_path,
        libitc_test_runner_ch_tgt,
        libitc_test_entrypoint_c_tgt,
        libitc_test_util_src,
        build_by_default: false,
        include_directories: [libitc_test_inc],
        dependencies: [
            libitc_test_dep,
            unity_dep,
            cmock_dep,
        ],
        c_args: libitc_test_c_args + common_c_args,
        link_args: libitc_test_link_args + common_link_args,
        install: true,
        install_dir: libitc_test_bin_install_dir
    )

    test(libitc_test_src_file, libitc_test_exe, verbose: true)
endforeach
