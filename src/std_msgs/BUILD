COPTS = [
    "-std=c++11"
]

cc_library(
    name = "std_msgs_lib",
    srcs = glob([
        "include/**/*.h",

        # "src/**/*.cpp"
    ]),
    includes = [
         "include"
    ],
    copts = COPTS,
    linkstatic = True,
    # deps = [
    #     "@com_github_google_glog//:glog",
    #     "//xcroscpp_core:xcroscpp_core_lib",
    # ],
    visibility = ["//visibility:public"],
)