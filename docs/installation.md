# Installation

Add the driver to a CMake or ESP-IDF project. It is a header-only C++17 library with no dependencies.

## Requirements

- A C++17 compiler. CI builds with GCC and Clang on Ubuntu 24.04, and with the ESP-IDF toolchain.
- CMake 3.16 or newer, which also generates the version header.
- No RTOS, heap or exceptions: every call is `noexcept` and returns `bool` or a value.

## Get the source

A git submodule keeps the driver pinned to a version you chose:

```bash
git submodule add https://github.com/hardfoc/hf-mcp9700-driver.git external/hf-mcp9700-driver
git submodule update --init external/hf-mcp9700-driver
```

Only `inc/`, `src/`, `cmake/` and `CMakeLists.txt` are needed to build. The examples have their own submodule
(`examples/esp32/scripts`); you do not need it to use the driver.

## CMake

```cmake
add_subdirectory(external/hf-mcp9700-driver)
target_link_libraries(my_app PRIVATE hf::mcp9700)
```

`hf::mcp9700` is an alias of the `hf_mcp9700` `INTERFACE` target. It carries both include directories (`inc/` and
the generated `mcp9700_version.h`) and requires C++17 (`cxx_std_17`), so your target does not need to set either.

| Option | Default | Effect |
| --- | --- | --- |
| `HF_MCP9700_ENABLE_WARNINGS` | `OFF` | Adds `-Wall -Wextra -Wpedantic` to targets that link the driver |
| `HF_MCP9700_BUILD_TESTS` | `OFF` | Builds the host unit tests in `tests/` ([Testing and CI](testing.md)) |

`find_package(hf_mcp9700)` is not supported yet: the build writes a package configuration but installs nothing.
Use `add_subdirectory()`.

## Without the root CMakeLists

Include [`cmake/hf_mcp9700_build_settings.cmake`](../cmake/hf_mcp9700_build_settings.cmake) and add
`HF_MCP9700_PUBLIC_INCLUDE_DIRS` to your target. Including the file is what generates `mcp9700_version.h` (into
`hf_mcp9700_generated/` under the current binary directory); `mcp9700_thermistor.hpp` does not compile without it.

```cmake
include(${CMAKE_CURRENT_SOURCE_DIR}/external/hf-mcp9700-driver/cmake/hf_mcp9700_build_settings.cmake)
target_include_directories(my_app PRIVATE ${HF_MCP9700_PUBLIC_INCLUDE_DIRS})
target_compile_features(my_app PRIVATE cxx_std_17)
```

## ESP-IDF

The ESP-IDF component wrapper is [`examples/esp32/components/hf_mcp9700/`](../examples/esp32/components/hf_mcp9700/CMakeLists.txt).
Copy that folder into your project's `components/` (or point `EXTRA_COMPONENT_DIRS` at it) and set the
`HF_MCP9700_ROOT` cache variable to the driver checkout; the wrapper stops with a clear message when the path is
wrong. Your component then declares `REQUIRES hf_mcp9700`.

## How the build is organised

| File | Role |
| --- | --- |
| [`cmake/hf_mcp9700_build_settings.cmake`](../cmake/hf_mcp9700_build_settings.cmake) | Include directories, version, IDF dependencies: the single source of truth |
| [`CMakeLists.txt`](../CMakeLists.txt) | `INTERFACE` target for host builds and CI |
| [`examples/esp32/components/hf_mcp9700/`](../examples/esp32/components/hf_mcp9700/CMakeLists.txt) | ESP-IDF component wrapper over the same settings |

The version is set once in the settings file and written into `mcp9700_version.h` at configure time, as
`HF_MCP9700_VERSION_MAJOR`, `_MINOR`, `_PATCH` and `HF_MCP9700_VERSION_STRING`. Read it at run time:

```cpp
const char* v = GetMcp9700DriverVersion();   // "1.0.0"
```

## Compile-time switches

There are none. The coefficients are constructor arguments, not macros ([Configuration](configuration.md)).

## Next

[Quickstart](quickstart.md) wires the sensor and reads a temperature; [Porting](porting.md) writes the ADC adapter
for your MCU.
