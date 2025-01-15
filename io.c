
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "ads1115.h"
#include "dac.h"
#include <inttypes.h>

// Define GPIO pins
#define GPIO_PIN_25 25
#define GPIO_PIN_26 26
#define GPIO_PIN_27 27
#define GPIO_PIN_14 14
#define GPIO_PIN_33 33
#define GPIO_PIN_32 32
#define GPIO_PIN_35 35
#define GPIO_PIN_34 34
// #define GPIO_NUM_0 0
#define i2c_port I2C_NUM_0
#define address 0x48


int dochannels[4] = {
        GPIO_PIN_25, // Channel 0
        GPIO_PIN_26, // Channel 1
        GPIO_PIN_27, // Channel 2
        GPIO_PIN_14  // Channel 3
    };

int dichannels[4] = {
        GPIO_PIN_33, // Channel 0
        GPIO_PIN_32, // Channel 1
        GPIO_PIN_35, // Channel 2
        GPIO_PIN_34  // Channel 3
    };



void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
    ESP_LOGI("I2C", "I2C initialized successfully");
    setup_gpio_interrupt(GPIO_NUM_0);
}




void dochannel(int i, bool set) {
    if(i>4)i=3;
    gpio_set_level(dochannels[i], set);

    ESP_LOGI("GPIO", "Digital Output channel:%d is set %d\n",i,set);
}


unsigned int dichannel(int i) {
    if(i>4)i=3;
    esp_rom_gpio_pad_select_gpio(dichannels[i]);
    gpio_set_direction(dichannels[i], GPIO_MODE_INPUT);
    gpio_pulldown_en(dichannels[i]);
    gpio_pullup_dis(dichannels[i]);
    gpio_set_intr_type(dichannels[i], GPIO_INTR_POSEDGE);
    for(int j=0;j<15;j++){    
        if(gpio_get_level(dichannels[i])==1){
        return 1;}
    }
    return 0;
}


void dacchannels(int channel, uint16_t value) {
    if(channel>4){channel = 3;}
    if(value> 4095){value = 4095;}
    esp_err_t ret = mcp4728_write_dac(channel,value);
    if (ret == ESP_OK) {
        printf("DAC channel %d: set successfully with value:%X\n", channel, value);
    } else {
        printf("Error setting DAC channel %d: %s\n",channel, esp_err_to_name(ret));
    }
    
}
double voltage;
double current;
ads1115_mux_t mux_settings[4] = {
    ADS1115_MUX_0_GND, // Channel 0
    ADS1115_MUX_1_GND, // Channel 1
    ADS1115_MUX_2_GND, // Channel 2
    ADS1115_MUX_3_GND  // Channel 3
};

float adcvoltage(int channel){
    ads1115_t ads = ads1115_config(i2c_port,address);
    ads1115_set_mux(&ads, mux_settings[channel]);
    printf("Setting up ADC for channel %d\n",channel);
    float voltage = ads1115_get_voltage(&ads);
    return voltage;
} 



void gpio_output_init() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_PIN_25) | (1ULL << GPIO_PIN_26) |
                        (1ULL << GPIO_PIN_27) | (1ULL << GPIO_PIN_14),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    ESP_LOGI("GPIO", "Output GPIO pins initialized");
}

void gpio_input_init() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_PIN_33) | (1ULL << GPIO_PIN_32) |
                        (1ULL << GPIO_PIN_35) | (1ULL << GPIO_PIN_34),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    ESP_LOGI("GPIO", "Input GPIO pins initialized");
}

void app_main() {
    i2c_master_init();
    gpio_output_init();
    gpio_input_init();

    

}
