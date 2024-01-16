#pragma once

#define SERIAL_BAUD 115200

// UART pins (for communicating with SIMCOM)
const int UART_RX = 16;
const int UART_TX = 15;

// i2c Pins (for communicating with Sense Hat)
const int I2C_SDA = 13;
const int I2C_SCL = 14;

// SD Card pins
// const int SD_SCK = 14;
// const int SD_MISO = 12;
// const int SD_MOSI = 13;
// const int SD_CS = 15;

// Controllable LED
const int BLINK_LED = 21;