# MCP9700 Driver API reference {#mainpage}

The classes, members and constants of the HardFOC MCP9700 driver, generated from the headers.

Start with hf::mcp9700::Mcp9700Thermistor, the driver: it reads a voltage through your ADC adapter and converts it
to degrees Celsius. The default coefficients are hf::mcp9700::kDefaultVoltsAtZeroC and
hf::mcp9700::kDefaultVoltsPerCelsius; GetMcp9700DriverVersion() returns the driver version.

The guides, with examples that compile, are on the documentation site: <a href="../">MCP9700 Driver
documentation</a>.
