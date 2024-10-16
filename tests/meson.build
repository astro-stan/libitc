subdir('mocked')
subdir('normal')

libitc_test_inc = include_directories([
    'include',
])

libitc_test_c_args = [
    '-Wno-sign-conversion',
]

libitc_test_link_args = [
    '-Wl,--gc-sections'
]

libitc_test_common_src = files([
    'ITC_TestUtil.c'
])
