---
layout: default
title: "🛠️ Installation"
description: "Install and integrate the HF-MCP9700 header-only driver"
nav_order: 3
parent: "📚 Documentation"
permalink: /docs/installation/
---

# Installation

This guide covers toolchain expectations, how to obtain the source, and how the **generated** `mcp9700_version.h` fits into your include path.

## Prerequisites

- **C++17** compiler (GCC 9+, Clang 9+, MSVC with `/std:c++17` or later) — see [cppreference](https://en.cppreference.com/w/cpp/17)
- **CMake 3.16+** if you use the provided root [`CMakeLists.txt`](https://github.com/hardfoc/hf-mcp9700-driver/blob/main/CMakeLists.txt) or [`cmake/hf_mcp9700_build_settings.cmake`](https://github.com/hardfoc/hf-mcp9700-driver/blob/main/cmake/hf_mcp9700_build_settings.cmake)

## Obtain the source

```bash
git clone --recursive https://github.com/hardfoc/hf-mcp9700-driver.git
cd hf-mcp9700-driver
```

For ESP32 examples, the recursive clone ensures [`examples/esp32/scripts`](https://github.com/hardfoc/hf-mcp9700-driver/tree/main/examples/esp32) is populated.

## Header-only layout

Add two include roots:

1. **`inc/`** — [`mcp9700_thermistor.hpp`](https://github.com/hardfoc/hf-mcp9700-driver/blob/main/inc/mcp9700_thermistor.hpp) and friends  
2. **CMake binary dir** where `mcp9700_version.h` is generated (see below)

Including [`cmake/hf_mcp9700_build_settings.cmake`](https://github.com/hardfoc/hf-mcp9700-driver/blob/main/cmake/hf_mcp9700_build_settings.cmake) runs `configure_file` on [`inc/mcp9700_version.h.in`](https://github.com/hardfoc/hf-mcp9700-driver/blob/main/inc/mcp9700_version.h.in) and emits:

`${CMAKE_CURRENT_BINARY_DIR}/hf_mcp9700_generated/mcp9700_version.h`

## CMake (recommended)

See the full walkthrough in **[CMake integration](cmake_integration.md)**. Minimal pattern:

```cmake
include("${CMAKE_SOURCE_DIR}/third_party/hf-mcp9700-driver/cmake/hf_mcp9700_build_settings.cmake")
target_include_directories(my_target PRIVATE ${HF_MCP9700_PUBLIC_INCLUDE_DIRS})
target_link_libraries(my_target PRIVATE hf::mcp9700)  # after add_subdirectory or FetchContent
```

## Verify

After configuration, build a target that includes:

```cpp
#include "mcp9700_thermistor.hpp"
```

If the compiler reports **`mcp9700_version.h` not found**, ensure the **generated** include directory is on your path — see [Troubleshooting](troubleshooting.md).

**Next:** [Quick start →](quickstart.md)
