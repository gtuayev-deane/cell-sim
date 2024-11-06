// esp32 library for TCA6408 tested with TCA6408ARGTR
// emanuele feola - 06 03 2024

/*
From datasheeet:
The TCA6408A consists of one 8-bit Configuration (input or output selection), Input, Output, and Polarity
Inversion (active high) Register. At power on, the I/Os are configured as inputs.
However, the system controller can enable the I/Os as either inputs or outputs by writing to the I/O configuration bits.

The data for each input or output is kept in the corresponding Input or Output Register.
The polarity of the Input Port Register can be inverted with the Polarity Inversion Register.
All registers can be read by the system controller.
*/

#pragma once
#include <Arduino.h>
#include <Wire.h>

#define I2C_TCA6408_ADDR 0x21

// TCA registers
#define TCA6408_INPUT 0x00
#define TCA6408_OUTPUT 0x01
#define TCA6408_POLARITY_INVERSION 0x02
#define TCA6408_CONFIGURATION 0x03

// TCA input/output
// #define TCA_INPUT_PIN 1
// #define TCA_OUTPUT_PIN 0

void TCA_Begin(int8_t sda_pin, int8_t scl_pin, uint32_t wire_freq, uint16_t wire_timeout_ms);
uint8_t TCA_Read(uint8_t reg);
uint8_t TCA_Write(uint8_t reg, uint8_t data);

uint8_t TCA_Read_Input();
uint8_t TCA_Read_Output();
uint8_t TCA_Read_Inversion();
uint8_t TCA_Read_Direction();

uint8_t TCA_Write_Output(uint8_t data);
uint8_t TCA_Write_Inversion(uint8_t data);
uint8_t TCA_Write_Direction(uint8_t data);

bool TCA_Set_Out_Pin(uint8_t pin_index, bool state);
bool TCA_Get_Input_Pin(uint8_t pin_index);