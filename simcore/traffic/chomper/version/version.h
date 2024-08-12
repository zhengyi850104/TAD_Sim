// Copyright 2024 Tencent Inc. All rights reserved.
//

#pragma once

#define VER_MAJOR 1
#define VER_MINOR 0
#define VER_BUILD 1
#define VER_REVISION

#define Stringizing(v) #v

// generate rights
#define COPYRIGHT_STR() \
  "Copyright (C) "      \
  "2023"

// generate version string
#define VER_STR(major, minor, build, revision)                                                                \
  "V" Stringizing(major) "." Stringizing(minor) "." Stringizing(build) "." Stringizing(revision) " " __DATE__ \
                                                                                                 " " __TIME__

// generate file version
#define FILE_VERSION_STR(branch, commit_id) \
  "V" Stringizing(branch) "." Stringizing(commit_id) "-" "20231103" "-" "112804"

// generate product version
#define PRODUCT_VERSION_STR(major, minor, build, revision) \
  "V" Stringizing(major) "." \
  Stringizing(minor) "." \
  Stringizing(build) "." \
  Stringizing(revision) "-" "20231103" "T" "112804"
