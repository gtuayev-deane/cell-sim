#include "Cell.h"

// Constructor
Cell::Cell(uint8_t mux_channel, TwoWire& wire_interface) :
    mux_channel(mux_channel), wire(wire_interface) {
    // ...
}

// Implementation of public methods
void Cell::init()
{
    // Set the mux channel
    setMuxChannel();

    // Initialize DACs
    ldo_dac.begin(LDO_ADDRESS, &wire);
    buck_dac.begin(BUCK_ADDRESS, &wire);

    // Initialize ADC
    adc.begin(ADC_ADDRESS, &wire);

    // Configure GPIO expander
    wire.beginTransmission(0x20);
    wire.write(0x03); // Configuration register
    wire.write(0x00); // Set all pins as output
    wire.endTransmission();

    // Set the outputs to 0
    wire.beginTransmission(0x20);
    wire.write(0x01); // Configuration register
    wire.write(0x00); // Set all pins as output
    wire.endTransmission();
}

void Cell::disable()
{
    setMuxChannel();
    GPIO_STATE &= ~(1 << gpio_buck_enable);
    GPIO_STATE &= ~(1 << gpio_ldo_enable);
    setGPIOState();
}

void Cell::enable()
{
    setMuxChannel();
    GPIO_STATE |= (1 << gpio_buck_enable);
    GPIO_STATE |= (1 << gpio_ldo_enable);
    setGPIOState();
}

float Cell::getVoltage()
{
    setMuxChannel();
    int16_t adc_value = adc.readADC_SingleEnded(adc_output_voltage);
    float volts = adc.computeVolts(adc_value);
    return volts;
}

void Cell::setVoltage(float voltage)
{
    // Set the mux channel
    setMuxChannel();

    float buck_voltage = voltage * 1.05;
    float ldo_voltage = voltage;

    // Buck voltage limits
    if (buck_voltage < MIN_BUCK_VOLTAGE) buck_voltage = MIN_BUCK_VOLTAGE;
    if (buck_voltage > MAX_BUCK_VOLTAGE) buck_voltage = MAX_BUCK_VOLTAGE;
    if (ldo_voltage < MIN_LDO_VOLTAGE) ldo_voltage = MIN_LDO_VOLTAGE;
    if (ldo_voltage > MAX_LDO_VOLTAGE) ldo_voltage = MAX_LDO_VOLTAGE;

    // Set the output voltage
    setBuckVoltage(buck_voltage);
    setLDOVoltage(ldo_voltage);
}

void Cell::turnOnOutputRelay()
{
    setMuxChannel();
    GPIO_STATE |= (1 << gpio_output_relay_control);
    setGPIOState();
}

void Cell::turnOffOutputRelay()
{
    setMuxChannel();
    GPIO_STATE &= ~(1 << gpio_output_relay_control);
    setGPIOState();
}

void Cell::turnOnLoadSwitch()
{
    setMuxChannel();
    GPIO_STATE |= (1 << gpio_load_switch_control);
    setGPIOState();
}

void Cell::turnOffLoadSwitch()
{
    setMuxChannel();
    GPIO_STATE &= ~(1 << gpio_load_switch_control);
    setGPIOState();
}

float Cell::getCurrent()
{
    setMuxChannel();
    return readShuntCurrent();
}

// Implementation of private methods

float Cell::getLDOVoltage()
{
    setMuxChannel();
    int16_t adc_value = adc.readADC_SingleEnded(adc_ldo_voltage);
    float volts = adc.computeVolts(adc_value);
    return volts;
}

uint16_t Cell::calculateSetpoint(float voltage, bool useBuckCalibration)
{
    // Select calibration points based on parameter
    const std::pair<float, float>* calibration_points = useBuckCalibration ? BUCK_SETPOINTS : LDO_SETPOINTS;

    // Calculate slope using the two points
    float m = (calibration_points[1].first - calibration_points[0].first) / 
              (calibration_points[1].second - calibration_points[0].second);

    // Calculate intercept
    float b = calibration_points[0].first - m * calibration_points[0].second;

    // Calculate and return the setpoint for the desired voltage
    return static_cast<uint16_t>(m * voltage + b);
}

float Cell::getBuckVoltage()
{
    setMuxChannel();
    int16_t adc_value = adc.readADC_SingleEnded(adc_buck_voltage);
    float volts = adc.computeVolts(adc_value);
    return volts;
}

void Cell::setBuckVoltage(float voltage)
{
    setMuxChannel();
    uint16_t setpoint = calculateSetpoint(voltage, true);


    buck_dac.setVoltage(setpoint, false);
}

void Cell::setLDOVoltage(float voltage)
{
    setMuxChannel();
    uint16_t setpoint = calculateSetpoint(voltage, false);
    ldo_dac.setVoltage(setpoint, false);
}
// Implementation of helper methods

void Cell::setGPIOState()
{
    setMuxChannel();
    // Set output relay on P7
    wire.beginTransmission(TCA6408_ADDR);
    wire.write(0x01); // Output register
    wire.write(GPIO_STATE);
    wire.endTransmission();
}

float Cell::readShuntCurrent()
{
    setMuxChannel();
    int16_t adc_value = adc.readADC_SingleEnded(adc_output_current);
    float volts = adc.computeVolts(adc_value);

    return volts / SHUNT_RESISTOR_OHMS / SHUNT_GAIN;
}

void Cell::setMuxChannel()
{
    wire.beginTransmission(MUX_ADDRESS);
    wire.write(1 << mux_channel);
    wire.endTransmission();
}
