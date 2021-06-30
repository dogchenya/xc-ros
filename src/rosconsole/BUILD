COPTS = [
    "-std=c++11"
]

cc_library(
    name = "rosconsole_lib",
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
        "@com_github_google_glog//:glog",
        "//xcroscpp_core:xcroscpp_core_lib",
        "@rosdep//:rosdep_lib",
    ],
    visibility = [
        "//xcroscpp:__pkg__",
    ]
)