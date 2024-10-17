libitc_test_mocked_libitc_header_paths = []
libitc_test_mocked_libitc_header_header_names = []
libitc_test_mocked_libitc_header_source_names = []

# Generate the mocks via the unity mock generator script
foreach libitc_test_mocked_header : libitc_test_mocked_libitc_headers
    # Hacky but there isn't simple way around it
    # Use the path of the first source of the lib file as the base dir
    base_dir = fs.parent(libitc_src[0].full_path())

    # Look for headers in the include dirs
    foreach libitc_inc_dir : libitc_inc_dirs
        final_header_path = base_dir / libitc_inc_dir / libitc_test_mocked_header

        if fs.is_file(final_header_path)
            libitc_test_mocked_libitc_header_paths += final_header_path
            libitc_test_mocked_libitc_header_header_names += 'Mock' + libitc_test_mocked_header
            libitc_test_mocked_libitc_header_source_names += 'Mock' + fs.replace_suffix(libitc_test_mocked_header, '.c')
        endif
    endforeach
endforeach

libitc_test_mock_ch_tgt = custom_target(
    ' Mock headers',
    build_by_default: true,
    build_always_stale: true,
    command: [
        bash,
        '-c',
        'cd "@OUTDIR@" && ' +
        ruby.full_path() + ' ' + unity_test_mock_generator_script + ' ' +
        '-o' + libitc_test_config_yaml_path + ' ' +
        ' '.join(libitc_test_mocked_libitc_header_paths)
    ],
    output: [libitc_test_mocked_libitc_header_source_names, libitc_test_mocked_libitc_header_header_names],
)

# Iterate over all unit test source files
foreach libitc_test_src_file_path : libitc_test_mocked_src
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
        'Mocked' + libitc_test_src_name.replace('_', ''),
        libitc_test_src_file_path,
        libitc_test_runner_ch_tgt,
        libitc_test_mock_ch_tgt,
        libitc_test_exes_common_arg,
        install: true,
        # Add run path to LibITC that is relative to the binary location
        install_rpath: '$ORIGIN' / fs.relative_to(
            get_option('prefix') / get_option('libdir'),
            get_option('prefix') / get_option('bindir'),
        ),
        kwargs: libitc_test_exes_common_kwargs
    )

    test(libitc_test_src_file, libitc_test_exe, verbose: true)
endforeach
