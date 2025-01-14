#ifndef DAC_H
#define DAC_H

// #define MCP4728_I2C_ADDRESS      0x60       // MCP4728 default I2C address
#include <stdio.h>
#include "driver/i2c.h"

esp_err_t mcp4728_write_dac(int channel, uint16_t value);
#endif