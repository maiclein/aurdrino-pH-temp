/*
 * file DFRobot_PH.ino
 * @ https://github.com/DFRobot/DFRobot_PH
 *
 * This is the sample code for Gravity: Analog pH Sensor / Meter Kit V2, SKU:SEN0161-V2
 * In order to guarantee precision, a temperature sensor such as DS18B20 is needed, to execute automatic temperature compensation.
 * You can send commands in the serial monitor to execute the calibration.
 * Serial Commands:
 *   enter -> enter the calibration mode
 *   cal -> calibrate with the standard buffer solution, two buffer solutions(4.0 and 7.0) will be automaticlly recognized
 *   exit -> save the calibrated parameters and exit from calibration mode
 *
 * Copyright   [DFRobot](http://www.dfrobot.com), 2018
 * Copyright   GNU Lesser General Public License
 *
 * version  V1.0
 * date  2018-04
 */

#include "DFRobot_PH.h"
#include <EEPROM.h>
#include <OneWire.h>

#define PH_PIN A1
int TEMP_PIN = 2; //DS18S20 Signal pin on digital 2

//Temperature chip i/o
OneWire ds(TEMP_PIN);  // on digital pin 2

float voltage,phValu;
// Declare ph as a /DFRobot_PH object
DFRobot_PH ph;

void setup()
{
  Serial.begin(115200);
  // Instantiate our ph object
  ph.begin();
}

void loop()
{
  float temperature;  
  float phValue;
  static unsigned long timepoint = millis();
  
  if(millis()-timepoint>1000U)  //time interval: 1s
  {
    timepoint = millis();
    voltage = analogRead(PH_PIN)/1024.0*5000;  // read the voltage
    temperature = readTemperature(); // read your temperature sensor to execute temperature compensation
    phValue = ph.readPH(voltage,temperature);  // convert voltage to pH with temperature compensation
    Serial.print("temperature:");
    Serial.print(temperature,1);
    Serial.print("^C  pH:");
    Serial.println(phValue,2);
  }
  ph.calibration(voltage,temperature);  // calibration process by Serail CMD
}

float readTemperature()
{
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Read Scratchpad


  for (int i = 0; i < 9;  i++  ) { // we need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;
}
