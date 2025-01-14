#ifndef IO_H
#define IO_H
#include <stdio.h>
#include "driver/i2c.h"
#include "driver/gpio.h"
float adcvoltage(int channel);
void dacchannels(int channel, uint16_t value);
int dichannel(int i);
void dochannel(int i, bool set);
#endif