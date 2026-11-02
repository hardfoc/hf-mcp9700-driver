/**
 * @file ports.cpp
 * @brief Instantiates the driver over the documented adapter, so every member compiles in full.
 */
#include "my_adc.hpp"

template class hf::mcp9700::Mcp9700Thermistor<MyAdc>;
