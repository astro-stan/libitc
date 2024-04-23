# Iterate over all unit test source files
foreach libitc_test_src_file_path : libitc_test_normal_src
    libitc_test_src_file = fs.name(libitc_test_src_file_path.full_path())
    libitc_test_src_name = fs.replace_suffix(libitc_test_src_file, '')

    libitc_test_runner_c = libitc_test_src_name + '_Runner.c'
    libitc_test_runner_h = libitc_test_src_name + '.h'
    libitc_test_runner_tgt_name = libitc_test_src_file + ' runner'

    # Generate the test runner via the unity runner generator script
    libitc_test_runner_ch_tgt = custom_target(
        libitc_test_runner_tgt_name,
        build_by_default: false,
        command: [
            ruby,
            unity_test_runner_generator_script,
            libitc_test_src_file_path.full_path(),
            '@OUTDIR@' / libitc_test_runner_c,
            libitc_test_config_yaml_path,
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
        libitc_test_exes_common_arg,
        install: true,
        install_dir: libitc_normal_test_bin_install_dir,
        # Add run path to LibITC that is relative to the binary location
        install_rpath: '$ORIGIN' / fs.relative_to(
            get_option('prefix') / libitc_install_dir,
            get_option('prefix') / libitc_normal_test_bin_install_dir,
        ),
        kwargs: libitc_test_exes_common_kwargs
    )

    test(libitc_test_src_file, libitc_test_exe, verbose: true)
endforeach
