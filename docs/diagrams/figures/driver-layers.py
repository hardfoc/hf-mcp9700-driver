"""The three layers of the driver: hardware-agnostic core, ADC adapter contract, platform port.

Reads: From top to bottom. Your application talks to one Mcp9700Thermistor<AdcType> object per sensor, whose
public calls (listed from inc/mcp9700_thermistor.hpp) initialise the ADC and return degrees Celsius. The core turns
one ADC voltage into a temperature with (V - v_zero_c) / v_per_c. The ADC adapter contract is the template
parameter: any class with the two methods named in the header's documentation, checked at compile time. The
platform port is the only code you write: that class, which brings your ADC up and returns the voltage at the
sensor's VOUT pin. Below it is the hardware.
"""

import re
from pathlib import Path

from hardfoc_mkdocs_theme.figkit import Fig

ROOT = Path(__file__).resolve().parents[3]
DRIVER = "inc/mcp9700_thermistor.hpp"


def data():
    drv = (ROOT / DRIVER).read_text()
    public = drv.split("public:", 1)[1].split("private:", 1)[0]
    calls = []
    for name in re.findall(r"^\s+(?:\[\[nodiscard\]\] )?(?:bool|float) (\w+)\(", public, re.M):
        if name not in calls:
            calls.append(name)
    contract = drv.split("@tparam AdcType", 1)[1].split("*/", 1)[0]
    hooks = re.findall(r"`(?:bool|int) (\w+)\(", contract)
    return {
        "calls": calls,
        "port_hooks": hooks,
        "sources": [f"{DRIVER}:Mcp9700Thermistor public calls", f"{DRIVER}:AdcType contract"],
    }


def build(d):
    f = Fig(
        "driver-layers",
        "Three layers, one of them yours",
        alt="Layer diagram: the application calls the hardware-agnostic MCP9700 driver, which relies on the ADC "
        "adapter contract, which the platform adapter you write fulfils, which reads the MCU's ADC connected to "
        "the MCP9700's analog output.",
        width=1200,
        kicker="Architecture",
        subtitle="The driver is the same on every MCU. The adapter is a small class with two methods.",
        profile="overview",
        legend=[("brand", "this library"), ("teal", "you write"), ("slate", "hardware")],
    )
    W = f.body_w
    app = f.card(0, 0, W, "Your application", ["thermal monitoring, derating, fan control ..."], tone="slate")
    top = app.b + 40
    chips_h = f.chips(16, f.region_inner_top(top), W - 32, [c + "()" for c in d["calls"]], tone="brand")
    reg = f.region(0, top, W, f.region_inner_top(top) - top + chips_h + 18, "Hardware-agnostic core", tone="brand",
                   sub="Mcp9700Thermistor<AdcType>, inc/mcp9700_thermistor.hpp")
    comm = f.card(0, reg.b + 40, W, "ADC adapter contract",
                  ["The AdcType template parameter: any class with these two methods, checked at compile time,",
                   "no base class; T = (V - v_zero_c) / v_per_c is applied in the core above it"],
                  tone="brand", accent=True)
    port = f.card(0, comm.b + 40, W, "Platform adapter (your class)", ["  ".join(h + "()" for h in d["port_hooks"])],
                  tone="teal", accent=True, mono=True)
    hw = f.card(0, port.b + 40, W, "MCU ADC and MCP9700",
                ["ADC input on VOUT: 100 mV to 1.75 V over −40 °C to +125 °C, 500 mV at 0 °C, 10 mV/°C"],
                tone="slate")
    for a, b in ((app, reg), (reg, comm), (comm, port), (port, hw)):
        f.connect(a, b, "bottom", "top", tone="rail")
    return f
