COPTS = [
    "-std=c++11"
]

cc_library(
    name = "xcroscpp_core_lib",
    srcs = glob([
        "cpp_common/include/**/*.h",
        "rostime/include/**/*.h",
        "roscpp_traits/include/**/*.h",
        "roscpp_serialization/include/**/*.h",

        "cpp_common/src/*.cpp",
        "rostime/src/*.cpp",
        "roscpp_traits/src/*.cpp",
        "roscpp_serialization/src/*.cpp"
    ]),
    includes = [
         "cpp_common/include",
         "rostime/include", 
         "roscpp_traits/include",
         "roscpp_serialization/include"
    ],
    copts = COPTS,
    linkstatic = True,
    visibility = [
        "//rosconsole:__pkg__",
        "//xcroscpp:__pkg__",
        ]
)