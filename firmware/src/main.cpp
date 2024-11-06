#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Cell.h>
// #include <Adafruit_SSD1306.h> // OLED
#include <Adafruit_MCP4725.h> // DAC
#include <Adafruit_ADS1X15.h> // ADC
#include <FastLED.h>          // Addressable LEDs


// Micro pinout
#define ledPin 0
#define sdaPin 7
#define sclPin 6

// Addressable LEDs
#define NUM_LEDS 6
CRGB leds[NUM_LEDS];

// Create cells
Cell cell1(1);
Cell cell2(2);
Cell cell3(3);

void setupLEDs()
{
    FastLED.addLeds<NEOPIXEL, ledPin>(leds, NUM_LEDS);
}

void updateStatusLEDs(float v1, float v2, float v3, float i1, float i2, float i3) {
    // Constants for max values
    const float MAX_VOLTAGE = 4.5;  // Maximum voltage
    const float MAX_CURRENT = 0.5;  // Maximum current in amps
    
    // Calculate brightness as fraction of maximum (0-255)
    uint8_t i1_bright = constrain((i1 / MAX_CURRENT) * 255, 0, 255);
    uint8_t i2_bright = constrain((i2 / MAX_CURRENT) * 255, 0, 255);
    uint8_t i3_bright = constrain((i3 / MAX_CURRENT) * 255, 0, 255);
    
    uint8_t v1_bright = constrain((v1 / MAX_VOLTAGE) * 255, 0, 255);
    uint8_t v2_bright = constrain((v2 / MAX_VOLTAGE) * 255, 0, 255);
    uint8_t v3_bright = constrain((v3 / MAX_VOLTAGE) * 255, 0, 255);

    // Set LED colors - Current LEDs are red, Voltage LEDs are blue
    // Cell 1
    leds[0] = CRGB(i1_bright, 0, 0);  // Current LED
    leds[1] = CRGB(0, 0, v1_bright);  // Voltage LED
    
    // Cell 2
    leds[2] = CRGB(i2_bright, 0, 0);  // Current LED
    leds[3] = CRGB(0, 0, v2_bright);  // Voltage LED
    
    // Cell 3
    leds[4] = CRGB(i3_bright, 0, 0);  // Current LED
    leds[5] = CRGB(0, 0, v3_bright);  // Voltage LED

    FastLED.show();
}

void setup()
{
    USBSerial.begin(115200);
    Wire.setPins(sdaPin, sclPin);
    Wire.begin();

    cell1.init();
    cell2.init();
    cell3.init();

    cell1.turnOnOutputRelay();
    cell2.turnOnOutputRelay();
    cell3.turnOnOutputRelay();

    cell1.turnOffDMMRelay();
    cell2.turnOffDMMRelay();
    cell3.turnOffDMMRelay();

    cell1.enable();
    cell2.enable();
    cell3.enable();

    FastLED.addLeds<NEOPIXEL, ledPin>(leds, NUM_LEDS);
    FastLED.setBrightness(255);
}

float voltage = 0.02;
float sum = 0.01;
void loop()
{

    static float voltage = 1.5;
    static float step = 0.05;  // 10mV steps

    // Update voltage for next iteration
    voltage += step;

    // Reverse direction at limits
    if (voltage >= 4.5 || voltage <= 0.4) {
        step = -step;
    }

    USBSerial.print(">Setpoint: ");
    USBSerial.println(voltage);

    cell1.setVoltage(voltage);
    cell2.setVoltage(voltage);
    cell3.setVoltage(voltage);

    delay(10);


    // Read current
    float cell1_current = cell1.getCurrent();
    float cell2_current = cell2.getCurrent();
    float cell3_current = cell3.getCurrent();

    USBSerial.print(">Cell 1 current(A): ");
    USBSerial.println(cell1_current, 5);
    USBSerial.print(">Cell 2 current(A): ");
    USBSerial.println(cell2_current, 5);
    USBSerial.print(">Cell 3 current(A): ");
    USBSerial.println(cell3_current, 5);

    // Read voltage
    float cell1_voltage = cell1.getVoltage();
    float cell2_voltage = cell2.getVoltage();
    float cell3_voltage = cell3.getVoltage();

    USBSerial.print(">Cell 1 voltage(V): ");
    USBSerial.println(cell1_voltage, 5);
    USBSerial.print(">Cell 2 voltage(V): ");
    USBSerial.println(cell2_voltage, 5);
    USBSerial.print(">Cell 3 voltage(V): ");
    USBSerial.println(cell3_voltage, 5);

    static unsigned long lastTime = micros();
    unsigned long currentTime = micros();
    unsigned long elapsedTime = currentTime - lastTime;
    lastTime = currentTime;

    // Get buck and LDO voltages
    float cell1_buck = cell1.getBuckVoltage();
    float cell2_buck = cell2.getBuckVoltage();
    float cell3_buck = cell3.getBuckVoltage();

    float cell1_ldo = cell1.getLDOVoltage(); 
    float cell2_ldo = cell2.getLDOVoltage();
    float cell3_ldo = cell3.getLDOVoltage();

    USBSerial.print(">Cell 1 buck(V): ");
    USBSerial.println(cell1_buck, 5);
    USBSerial.print(">Cell 2 buck(V): ");
    USBSerial.println(cell2_buck, 5);
    USBSerial.print(">Cell 3 buck(V): ");
    USBSerial.println(cell3_buck, 5);

    USBSerial.print(">Cell 1 LDO(V): ");
    USBSerial.println(cell1_ldo, 5);
    USBSerial.print(">Cell 2 LDO(V): ");
    USBSerial.println(cell2_ldo, 5);
    USBSerial.print(">Cell 3 LDO(V): ");
    USBSerial.println(cell3_ldo, 5);

    USBSerial.print(">Loop time(us): ");
    USBSerial.println(elapsedTime);

    // Update status LEDs
    updateStatusLEDs(
        cell1_voltage, cell2_voltage, cell3_voltage,
        cell1_current, cell2_current, cell3_current
    );
}


