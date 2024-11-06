#include <Arduino.h>
#include "TCA6408.h"

#define TCA_IO_ARR 0b10000001 // define input output of the 8 available pins
#define I2C_SDA 21
#define I2C_SCL 22

void setup()
{
  Serial.begin(115200);

  TCA_Begin(I2C_SDA, I2C_SCL, 100000, 100);
  TCA_Write_Direction(TCA_IO_ARR); // set input/output pins
  bool pin_state = false;

  // test tca by toggling/reading pins
  while (1)
  {
    pin_state = !pin_state;
    TCA_Set_Out_Pin(1, pin_state);
    TCA_Set_Out_Pin(2, pin_state);
    TCA_Set_Out_Pin(3, pin_state);
    TCA_Set_Out_Pin(4, pin_state);
    TCA_Set_Out_Pin(5, pin_state);
    TCA_Set_Out_Pin(6, pin_state);

    bool pin0_read_val = TCA_Get_Input_Pin(0);
    Serial.printf("pin0_read_val: %d\n", pin0_read_val);

    bool pin7_read_val = TCA_Get_Input_Pin(7);
    Serial.printf("pin7_read_val: %d\n", pin7_read_val);

    vTaskDelay(100);
  }
}

void loop()
{
  vTaskDelete(NULL);
}