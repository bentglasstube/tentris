package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "tentris",
    data = ["//content"],
    linkopts = [
        "-lSDL2",
        "-lSDL2_image",
        "-lSDL2_mixer",
    ],
    srcs = ["main.cc"],
    deps = [
        "@libgam//:game",
        ":screens",
    ],
)

cc_library(
    name = "screens",
    srcs = [
        "title_screen.cc",
    ],
    hdrs = [
        "title_screen.h",
    ],
    deps = [
        "@libgam//:backdrop",
        "@libgam//:screen",
        "@libgam//:text",
        "@libgam//:util",
        ":piece",
    ],
)

cc_library(
    name = "piece",
    srcs = ["piece.cc"],
    hdrs = ["piece.h"],
    deps = [
      "@libgam//:graphics",
      "@libgam//:spritemap",
    ],
)
