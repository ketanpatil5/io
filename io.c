
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "ads1115.h"
#include "dac.h"

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


// static QueueHandle_t gpio_evt_queue NULL;
// static void IRAM_ATTR gpio_isr_handler(void*arg){
//     uint32_t gpio_num = (uint32_t) arg;
//     xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);

// }
// static void gpio_task_example(void*arg){
//     uint32_t io_num;
//     for(;;){
//         if(xQueueSendFromISR(gpio_evt_queue, &io_num, portMAX_DELAY);){
//             printf("GPIO[%"PRIu32"] intr, val:%d\n",io_num, gpio_get_level(io_num));
//         }
//     }

// }


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

void dochannel(int i, bool set) {
    if(i>4)i=4;
    gpio_set_level(dochannels[i], set);


    ESP_LOGI("GPIO", "Digital Output channel:%d is set %d\n",i,set);
}

int dichannel(int i) {
    // if (i >= 4 || i < 0) {
    //     ESP_LOGE("GPIO", "Invalid channel index: %d", i);
    //     return -1; // Error: Invalid channel index
    // }

    int didata[4]; // Array to store digital input states

    // Read input levels for all channels
    for (int j = 0; j < 4; j++) {
        didata[j] = gpio_get_level(dichannels[j]);
    }

    ESP_LOGI("GPIO", "Digital Input Data for channel %d: %d", i, didata[i]);
    return didata[i]; // Return the state of the requested channel

    // while (true) {
    //     for (int i = 0; i < num_pins; i++) {
    //         int level = gpio_get_level(input_pins[i]);
    //         ESP_LOGI(TAG, "GPIO %d level: %d", input_pins[i], level);
    //     }
    //     vTaskDelay(pdMS_TO_TICKS(1000)); // Delay 1 second
    // }
}


void dacchannels(int channel, uint16_t value) {
    if(channel>4)channel=4;
    if(value>4095)value=4095;
    esp_err_t ret = mcp4728_write_dac(channel,value);
    if (ret == ESP_OK) {
        printf("DAC channel %d: set successfully with value:%X\n",channel,value);
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
    while(1){
        int i;
        for(i = 0; i < 4; i++) {
            // dochannel(i, true);
        //    int val= dichannel(i);
            dacchannels(i, 2048);
            // voltage = adcvoltage(i);
            // printf("Voltage for channel %d: %d\n",i,val);
            // dochannel(i, 0);
        }
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        i=0;
        for(i = 0; i < 4; i++) {
            // dochannel(i, true);
        //    int val= dichannel(i);
            dacchannels(i, 0);
            // voltage = adcvoltage(i);
            // printf("Voltage for channel %d: %d\n",i,val);
            // dochannel(i, 0);
        }
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        i=0;
      

    }

}
