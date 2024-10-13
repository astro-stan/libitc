libitc_c_args = [

]

if get_option('tests')
    # Enforce strict max stack usage
    # This is done to ensure the stack usage of the library is as low as
    # possible, however, it is only enforced while unit testing, to ensure
    # maximum compatibility with different systems, architectures, and
    # compilers.
    libitc_c_args += '-Wstack-usage=144'
endif

libitc_link_args = [

]

libitc_pub_inc_dirs = [
    'include',
]
libitc_inc = include_directories(libitc_pub_inc_dirs)

libitc_pkg_inc_dirs = [
    'package-include',
]
libitc_pkg_inc = include_directories(libitc_pkg_inc_dirs)

# Needed for mocking
libitc_inc_dirs = libitc_pub_inc_dirs + libitc_pkg_inc_dirs

libitc_src = files([
    'ITC_Id.c',
    'ITC_Event.c',
    'ITC_Stamp.c',
    'ITC_Port.c',
    'ITC_SerDes_Util.c',
])
