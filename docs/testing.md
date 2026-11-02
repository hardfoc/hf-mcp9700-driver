# Testing and CI

How the driver is checked: host unit tests against a scripted ADC, compiled documentation, ESP32 builds and
static analysis, on every pull request.

## Host unit tests

The tests build the real driver against a mock adapter that returns a set voltage or error code and records
every call. They check the transfer function at the ends and middle of the datasheet's output range, custom
coefficients, argument checks, error propagation and the initialised state.

```bash
cmake -S . -B build-host -DHF_MCP9700_BUILD_TESTS=ON
cmake --build build-host
ctest --test-dir build-host --output-on-failure
```

| File | Covers |
| --- | --- |
| [`tests/test_driver.cpp`](../tests/test_driver.cpp) | Defaults, initialisation, the transfer function, coefficients, errors, version |
| [`tests/mock_adc.hpp`](../tests/mock_adc.hpp) | The scripted ADC adapter |
| [`tests/test_harness.hpp`](../tests/test_harness.hpp) | A dependency-free test registry and check macros |

## Documentation snippets

Every C++ block on this site is included from [`tests/docs/`](../tests/docs/my_adc.hpp), which the host build
compiles (target `hf_mcp9700_doc_snippets`) with the driver instantiated over the documented adapter. A change to
the API that breaks a page breaks the build first.

## CI workflows

| Workflow | What it checks |
| --- | --- |
| Host unit tests | GCC and Clang build and run the tests and the doc snippets |
| ESP32 examples build | Every app in `examples/esp32/app_config.yml`, for each listed ESP-IDF version and build type |
| C++ lint, C++ analysis | clang-format, clang-tidy and cppcheck |
| Docs | Strict site build, figure and generated-table drift, Doxygen, link check, Lighthouse budget |
| Markdown lint, YAML lint, link check | Repository text files |

The workflows call the shared, versioned workflows of
[hf-general-ci-tools](https://github.com/hardfoc/hf-general-ci-tools) and
[hf-espidf-ci-tools](https://github.com/hardfoc/hf-espidf-ci-tools).

## On hardware

The [ESP32 example](examples.md) is the hardware check: compare its logged temperature with a reference
thermometer next to the sensor, and its VOUT with a multimeter.
