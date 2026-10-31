---
layout: default
title: "⚙️ CMake integration"
description: "Embed hf-mcp9700-driver in CMake projects"
nav_order: 7
parent: "📚 Documentation"
permalink: /docs/cmake_integration/
---

# CMake integration

## Embedded in a larger project (HardFOC pattern)

```cmake
include("${CMAKE_SOURCE_DIR}/path/to/hf-mcp9700-driver/cmake/hf_mcp9700_build_settings.cmake")
target_include_directories(my_target PRIVATE ${HF_MCP9700_PUBLIC_INCLUDE_DIRS})
```

`HF_MCP9700_PUBLIC_INCLUDE_DIRS` contains:

- The [`inc/`](https://github.com/hardfoc/hf-mcp9700-driver/tree/main/inc) directory  
- The generated headers directory with [`mcp9700_version.h`](https://github.com/hardfoc/hf-mcp9700-driver/blob/main/inc/mcp9700_version.h.in)  

After `add_subdirectory(hf-mcp9700-driver)` (or equivalent), link:

```cmake
target_link_libraries(my_target PRIVATE hf::mcp9700)
```

## Standalone interface library

From the driver root:

```bash
cmake -S . -B build
cmake --build build
```

This exposes target **`hf_mcp9700`** and alias **`hf::mcp9700`** as an **`INTERFACE`** library (header-only).

## Related

- [Installation](installation.md) — include path checklist  
- [API reference](api_reference.md) — when you only need includes, not a link library  
