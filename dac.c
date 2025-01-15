#include "driver/i2c.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "dac.h"
#include "ads1115.h"

#define I2C_MASTER_NUM           I2C_NUM_0  // I2C port number
#define I2C_MASTER_SCL_IO        22         // GPIO for SCL
#define I2C_MASTER_SDA_IO        21         // GPIO for SDA
#define I2C_MASTER_FREQ_HZ       400000     // I2C clock frequency
#define MCP4728_I2C_ADDRESS      0x60       // MCP4728 default I2C address


esp_err_t mcp4728_write_dac(int channel, uint16_t value) {
uint8_t command;
switch (channel) { // Select channel
    case 0:
        command = 0x43;
        break;
    case 1:
        command = 0x40;
        break;
    case 2:
        command = 0x46;
        break;
    case 3:
        command = 0x44;
        break;
    default:
        command = 0x40;  // Default value if no case matches
        break;
}

    uint8_t high_byte = (value >> 8) & 0x0F;  // Upper 4 bits of 12-bit value
    uint8_t low_byte = value & 0xFF;          // Lower 8 bits

    printf("Command: 0x%02X, High Byte: 0x%02X, Low Byte: 0x%02X\n", command, high_byte, low_byte);

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MCP4728_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, command, true);
    i2c_master_write_byte(cmd, high_byte, true);
    i2c_master_write_byte(cmd, low_byte, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        printf("I2C command failed: %s\n", esp_err_to_name(ret));
    }
    return ret;
}



