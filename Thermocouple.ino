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
MAX31855 *thermocouple[] = {new MAX31855(MAXCLK, MAXCS1, MAXDO, 'T'),new MAX31855(MAXCLK, MAXCS2, MAXDO, 'T'),new MAX31855(MAXCLK, MAXCS3, MAXDO, 'T'),new MAX31855(MAXCLK, MAXCS4, MAXDO, 'T')};

// Initialize our LCD screen which is connected to digital pin 7 on our Arduino.
SoftwareSerial LCD = SoftwareSerial(255,7);


void setup() {

  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);

  for (int i=0; i<4; i++)
  {
    (*thermocouple[i]).begin();
  }
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
    int32_t bits = (*thermocouple[i]).readBits();
    String fault = (*thermocouple[i]).readError(bits);
    Serial.println(fault);
    LCD.write(12);
    LCD.print(fault);
  }

  // Slight delay so the error messages can be read
  delay(5000);
  // Let's clear the screen in preparation
  LCD.write(12);
}

void loop() {

  // Instantiating our low pass filters, which are weighted moving averages that use a weight, alpha, to determine how much of the previous value to carry forward.
  //LowPassFilter *filter[] = {new LowPassFilter(0.4),new LowPassFilter(0.4),new LowPassFilter(0.4),new LowPassFilter(0.4)};

  // Setup serial and LCD for print operationos
  Serial.println();
  LCD.write(12);

  // Time to loop through our pointer array for the thermocouples.
  for (int i=0; i<4; i++)
  {
    double temp = 0;
    double therm = 0;
    int32_t bits = 0;

    bits = (*thermocouple[i]).readBits();
    temp = (*thermocouple[i]).correctedTempCelsius(bits);
    therm = (*thermocouple[i]).readInternal(bits);
    //double filteredTemp = (*filter[i]).filter(temp);
    String tempString = "Sensor " + String(i) + ": " + String(temp);
    Serial.print(temp);
    Serial.print("\t");
    LCD.print(tempString);
    LCD.write(13);
  }
  delay(1000);
}
