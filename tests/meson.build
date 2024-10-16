subdir('mocked')
subdir('normal')

libitc_test_inc = include_directories([
    'include',
])

libitc_test_c_args = [
    # The generated test runner by Unity fails to compile because it casts
    # an integer to an unsigned integer:
    #
    # ```
    # ...
    # Unity.CurrentTestLineNumber = line_num;
    # ...
    # ```
    # To allow the tests to compile, distable warnings (errors if using `-Wall`)
    # on casts between signed an unsigned integers when compiling the test code.
    '-Wno-sign-conversion',
]

libitc_test_link_args = [
    '-Wl,--gc-sections'
]

libitc_test_common_src = files([
    'ITC_TestUtil.c'
])
