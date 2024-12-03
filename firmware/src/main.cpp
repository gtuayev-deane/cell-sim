#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Cell.h>
// #include <Adafruit_SSD1306.h> // OLED
#include <Adafruit_MCP4725.h> // DAC
#include <Adafruit_ADS1X15.h> // ADC
#include <FastLED.h>          // Addressable LEDs

// Pin definitions

// LED
const int ledPin = 15;

// I2C
const int wire_1_sdaPin = 7;
const int wire_1_sclPin = 6;
const int wire_2_sdaPin = 17;
const int wire_2_sclPin = 18;

// SPI2
const int spi_2_sckPin = 12;
const int spi_2_mosiPin = 11;
const int spi_2_misoPin = 13;
const int spi_2_csPin = 10;

// DMM MUX
const int DMM_MUX_PINS[] = {1, 2, 3, 4};
const int DMM_MUX_ENABLE = 5;

// Addressable LEDs
#define NUM_LEDS 16
CRGB leds[NUM_LEDS];

// Create cells
Cell cell1(0);
Cell cell2(1);
Cell cell3(2);
Cell cell4(3);
Cell cell5(4);
Cell cell6(5);
Cell cell7(6);
Cell cell8(7);

// Cell array
Cell cells[] = {cell1, cell2, cell3, cell4, cell5, cell6, cell7, cell8};


void setupLEDs()
{
    FastLED.addLeds<NEOPIXEL, ledPin>(leds, NUM_LEDS);
}

void updateStatusLEDs(Cell* cells, size_t num_cells) {
    // Constants for max values
    const float MAX_VOLTAGE = 4.5;  // Maximum voltage
    const float MAX_CURRENT = 0.5;  // Maximum current in amps
    
    // Update LEDs for each cell (2 LEDs per cell)
    for (size_t i = 0; i < num_cells && i*2 < NUM_LEDS; i++) {
        float voltage = cells[i].getVoltage();
        float current = cells[i].getCurrent();
        
        // Calculate brightness as fraction of maximum (0-255)
        uint8_t current_bright = constrain((current / MAX_CURRENT) * 255, 0, 255);
        uint8_t voltage_bright = constrain((voltage / MAX_VOLTAGE) * 255, 0, 255);

        // Set LED colors - Current LED is red, Voltage LED is blue
        leds[i*2] = CRGB(current_bright, 0, 0);     // Current LED
        leds[i*2 + 1] = CRGB(0, 0, voltage_bright); // Voltage LED
    }

    FastLED.show();
}

void setup()
{
    USBSerial.begin(115200);
    Wire.setPins(wire_1_sdaPin, wire_1_sclPin);
    Wire.begin();
    Wire1.setPins(wire_2_sdaPin, wire_2_sclPin);
    Wire1.begin();

    // SPI2
    // SPI.begin(12, 13, 11, 10); // SCK, MISO, MOSI, CS
    // SPI.setClockDivider(SPI_CLOCK_DIV8);
    // SPI.setBitOrder(MSBFIRST);
    // SPI.setDataMode(SPI_MODE0);

    delay(1000);

    // Initialize cells
    for (Cell& cell : cells) {
        cell.init();
        cell.enable();
        cell.turnOnOutputRelay();
        delay(10);
    }

    FastLED.addLeds<NEOPIXEL, ledPin>(leds, NUM_LEDS);
    FastLED.setBrightness(255);
}

float voltage = 2;
float sum = 0.01;
void loop()
{
    for (Cell& cell : cells) {
        cell.setVoltage(voltage);
    }

    updateStatusLEDs(cells, 8);
}