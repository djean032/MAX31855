/*
Thermocouple.ino - Sketch used to control 4 thermocouples with MAX31855 amplifiers.

Created by Dairen Jean 6MAY23.
*/


#include <SPI.h>
#include <SoftwareSerial.h>
#include "MAX31855.h"


/*
Define the pins for the MAX31855

For the data and clock pins, use the standard Arduino pins MISO (Master In Slave Out) and SCK (clock)

For the slave select or chip select pins, any of the digital pins work. In this example, 2-5 are chosen.

*/
#define MAXDO   MISO
#define MAXCS1  2
#define MAXCS2  3
#define MAXCS3  4
#define MAXCS4  5
#define MAXCLK  SCK

// Initialize the thermocouples using an array of pointers to their location, so we can iterate over them in our loop function.
MAX31855 *thermocouple[] = {new MAX31855(MAXCLK, MAXCS1, MAXDO, "T"),new MAX31855(MAXCLK, MAXCS2, MAXDO, "T"),new MAX31855(MAXCLK, MAXCS3, MAXDO, "T"),new MAX31855(MAXCLK, MAXCS4, MAXDO, "T")};

// Initialize our LCD screen which is connected to digital pin 7 on our Arduino.
SoftwareSerial LCD = SoftwareSerial(255,7);


void setup() {

  // Initializing our LCD
  pinMode(7,OUTPUT);
  digitalWrite(7,HIGH);


  // Initializing our Serial communications
  Serial.begin(9600);
  LCD.begin(9600);

  // Turns display on, with no cursor and no blink
  LCD.write(22);

  // Turns backlight on.
  LCD.write(17);


  // Fault checking each of our thermocouples.
  for (int i=0; i<4; i++)
  {
    String fault = (*thermocouple[i]).readError();
    Serial.println(fault);
    LCD.write(12 + i);
    LCD.print(fault);
  }

  // Slight delay so the error messages can be read
  delay(5000);
  // Let's clear the screen in preparation
  LCD.write(12);
}

void loop() {

  // Instantiating our low pass filters, which are weighted moving averages that use a weight, alpha, to determine how much of the previous value to carry forward.
  LowPassFilter *filter[] = {new LowPassFilter(0.75),new LowPassFilter(0.75),new LowPassFilter(0.75),new LowPassFilter(0.75)};

  // Setup serial and LCD for print operationos
  Serial.println();
  LCD.write(12);

  // Time to loop through our pointer array for the thermocouples.
  for (int i=0; i<4; i++)
  {
    double temp = (*thermocouple[i]).correctedTempCelsius();
    double filteredTemp = (*filter[i]).filter(temp);
    String fault = (*thermocouple[i]).readError();
    if (fault == "No faults detected.")
    {
      String tempString = "Sensor " + String(i) + ": " + String(filteredTemp);
      Serial.print(filteredTemp);
      Serial.print("\t");
      LCD.print(tempString);
      LCD.write(13);
    }
    else
    {
      String faultString = "Sensor " + String(i) + ": " + String(fault);
      Serial.print(fault);
      Serial.print("\t");
      LCD.print(faultString);
      LCD.write(13);
    }
  }

   delay(1000);
}