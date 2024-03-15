#include <Arduino.h>
#include <Wire.h>

#define I2C_ADDR 8

void i2cRequestEvent();

void setup() {
  Wire.begin(I2C_ADDR);
  Wire.onRequest(i2cRequestEvent);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void i2cRequestEvent() {
  Wire.write("hello");
}