/************************************************************************
Project3.ino

Read values from FPGA and Thermistor to display on both Serial Window and LCD Screen with Arduino

Version: 0.5
Apr 15, 2018
************************************************************************/

#include <LiquidCrystal.h>
#include <stdint.h>
#include "SparkFunBME280.h"
#include "Wire.h"
#include <NeoTee.h>
#include <Math.h>
// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;

int ThermistorPin = A1;
int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
unsigned long delayTime;
double out, input_A, input_D, in_D;
bool on = false;

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//Global sensor object
BME280 mySensor;

//Print out Data cleanly
Print *outputs[] = { &Serial, &lcd };  // <--  list all the output destinations here
NeoTee tee( outputs, sizeof(outputs)/sizeof(outputs[0]) );

void setup() {
  Serial.begin(9600);
  
  mySensor.settings.commInterface = I2C_MODE;
  mySensor.settings.I2CAddress = 0x76;
  mySensor.settings.runMode = 3; //Normal mode
  mySensor.settings.tStandby = 0;
  mySensor.settings.filter = 0;
  mySensor.settings.tempOverSample = 1;
  mySensor.settings.pressOverSample = 1;
  mySensor.settings.humidOverSample = 1;
  
  Serial.print("Program Started\n");
  Serial.print("Starting BME280... result of .begin(): 0x");
  //Calling .begin() causes the settings to be loaded
  delay(10);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
  Serial.println(mySensor.begin(), HEX);
  Serial.print("Displaying ID, reset and ctrl regs\n");
  Serial.print("ID(0xD0): 0x");
  Serial.println(mySensor.readRegister(BME280_CHIP_ID_REG), HEX);
  Serial.print("Reset register(0xE0): 0x");
  Serial.println(mySensor.readRegister(BME280_RST_REG), HEX);
  Serial.print("ctrl_meas(0xF4): 0x");
  Serial.println(mySensor.readRegister(BME280_CTRL_MEAS_REG), HEX);
  Serial.print("ctrl_hum(0xF2): 0x");
  Serial.println(mySensor.readRegister(BME280_CTRL_HUMIDITY_REG), HEX);
  
  uint8_t memCounter = 0x80;
  uint8_t tempReadData;
    
  lcd.begin(16, 2);              // start the library
  lcd.setCursor(0,0);
  
  delayTime = 1000;
  Serial.println();
}

void loop() {
  printValues();
  delay(delayTime);      
  lcd.clear();
} //Loop end

void printValues() {
  int num;
  int i;
  byte value;
  int n0 = digitalRead(22),
  n1 = digitalRead(23),
  n2 = digitalRead(24),
  n3 = digitalRead(25),
  n4 = digitalRead(26),
  n5 = digitalRead(27),
  n6 = digitalRead(28),
  n7 = digitalRead(39);
  int array1[] = {n7,n6,n5,n4,n3,n2,n1,n0};  
  for ( int8_t i = 7; i >= 0; i-- ) value |= array1[i] << i;
  num = value;
  
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;
  in_D = num;
  input_A = T*T*T*T,1;
  input_D = in_D*in_D*in_D*in_D;
  out = input_A + input_D;
 
  lcd.setCursor(0,0);
  tee.print("AT:");
  tee.print(T, 1);   
  tee.print(" RNG:");
  tee.println(num, 1);
  lcd.setCursor(0,1);
  tee.print("Output:");
  tee.println(out);
  
  Serial.println("");
  Serial.print("Binary Value From FPGA: ");
  Serial.print(n0);
  Serial.print(n1);
  Serial.print(n2);
  Serial.print(n3);
  Serial.print(n4);
  Serial.print(n5);
  Serial.print(n6);
  Serial.println(n7);
  Serial.print("Decimal of the Random Number from FPGA: ");
  Serial.println(value);

  Serial.print("Digital Temperature: ");
  Serial.print(mySensor.readTempC(), 1);
  Serial.println(" C");

  Serial.print("Pressure: ");
  Serial.print(mySensor.readFloatPressure(), 2);
  Serial.println(" Pa");

  Serial.print("Altitude: ");
  Serial.print(mySensor.readFloatAltitudeMeters(), 2);
  Serial.println(" m");

	Serial.println();
}

