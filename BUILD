load("@rules_proto//proto:defs.bzl", "proto_library")

cc_proto_library(
    name = "sharedstreets_pb_cc",
    deps = [
        "@sharedstreets-ref-system//:sharedstreets_proto",
    ],
)

cc_library(
    name = "sharedstreets_tile_parser",
    srcs = [
        "sharedstreets-tile-parser.cc",
    ],
    hdrs = ["sharedstreets-tile-parser.h"],
    deps = [
        ":sharedstreets_pb_cc",
    ]
)

cc_binary(
    name = "shst_tile_info",
    srcs = ["shst_tile_info.cc",],
    deps = [
        ":sharedstreets_pb_cc",
        ":sharedstreets_tile_parser",
    ]
)
