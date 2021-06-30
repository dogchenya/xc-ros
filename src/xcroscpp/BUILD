COPTS = [
    "-std=c++11"
]

cc_library(
    name = "xcroscpp_lib",
    srcs = glob([
        "include/**/*.h",

        "src/**/*.cpp"
    ]),
    includes = [
         "include"
    ],
    copts = COPTS,
    linkstatic = True,
    deps = [
        "//common:common_lib",
        "//xcroscpp_core:xcroscpp_core_lib",
        "//rosconsole:rosconsole_lib",
        "//robotrpcpp:robotrpcpp_lib",
        "//std_msgs:std_msgs_lib",
        "@rosdep//:rosdep_lib",
    ]
)