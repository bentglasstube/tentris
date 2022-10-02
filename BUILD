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
    "game_screen.cc",
    "stats_screen.cc",
    "title_screen.cc",
  ],
  hdrs = [
    "game_screen.h",
    "stats_screen.h",
    "title_screen.h",
  ],
  deps = [
    "@libgam//:backdrop",
    "@libgam//:screen",
    "@libgam//:sprite",
    "@libgam//:spritemap",
    "@libgam//:text",
    "@libgam//:util",
    ":floater",
    ":piece",
    ":stats",
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

cc_library(
  name = "floater",
  srcs = ["floater.cc"],
  hdrs = ["floater.h"],
  deps = [
    "@libgam//:graphics",
    "@libgam//:spritemap",
  ],
)

cc_library(
  name = "stats",
  srcs = ["stats.cc"],
  hdrs = ["stats.h"],
)
