idf_component_register(SRCS "io.c" "ads1115.c" "dac.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_driver_gpio 
                       REQUIRES driver
                       )
