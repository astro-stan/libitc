libitc_c_args = [

]

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
    'ITC_SerDes.c',
    'ITC_Port.c',
])
