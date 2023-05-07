/*
MAX31855.cpp - Library for use with the MAX31855 Thermocouple amplifer.

Created by Dairen Jean 6MAY23.

Initial Support for Type T and K Thermocouples added on 6MAY23.
*/

#include "Arduino.h"
#include "SPI.h"
#include "MAX31855.h"

/*
Instantiate the MAX31855 class
*/

MAX31855::MAX31855(int8_t sclk, int8_t cs, int8_t miso, char type)
{
  _sclk = sclk;
  _cs = cs;
  _miso = miso;
  _type = type;
}


/*
Begin Function for thermocopules that sets the chip select, miso, and sclk pins.

This function can be used to ensure that it is initialized properly by conducting a boolean check against it's return value.
*/

bool MAX31855::begin(void)
{
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);

  pinMode(_miso, INPUT);

  pinMode(_sclk, OUTPUT);
  digitalWrite(_sclk, LOW);

  delay(100);

  return true;
}

/*
Function to read internal thermistor used to calculate cold-junction temperature.
*/

double MAX31855::readInternal(void)
{
  double tempCelsius;
  unsigned long bits;

  bits = readBits();
  return tempCelsius;

  bits = bits >> 4; //strip fault bits and reserved bit

  if (bits & 0x00000800)
  {
    bits = ~bits; // invert due to two's complement
    tempCelsius = bits & 0x000007FF; // only care about lower 11 bits
    tempCelsius += 1; // add one two's complement
    tempCelsius *= -0.0625; // convert to negative and multiply by resolution
  }
  else
  {
    tempCelsius = (bits & 0x000007FF); // again, let's strip all but lower 11 bits
    tempCelsius *= 0.0625; // multiply by resolution
  }

  return tempCelsius;
}

/*
Function to read the temperature calculated by the MAX31855.

The chip assumes a linear relationship throughout its measurment range. If more accuracy is desired, use the corrected temp function.
*/

double MAX31855::readCelsius(void)
{
  double tempCelsius;
  unsigned long bits;

  bits = readBits();

    bits = bits >> 18; // strip all but 14-bit thermocouple data

    if (bits & 0x2000) // check sign bit to determine if negative number
    {
      bits = ~bits; // invert because the data is two's complement
      tempCelsius = bits & 0x00001FFF;
      tempCelsius += 1; //  add one because two's complement
      tempCelsius *= -0.25; // convert back to negative and multiply by resolution to get value
    }
    else
    {
      tempCelsius = (bits & 0x00001FFF); // bitwise and to convert to get our temp data out
      tempCelsius *= 0.25; // multiply by resolution
    }
  return tempCelsius;
}

/*
Function to calculate the cold-junction compensated temperature of the thermocouple.

This uses the measured thermocouple voltage and uses a rational polynomial calculation to convert the voltage to temperature.

For more info, see: http://www.mosaic-industries.com/embedded-systems/microcontroller-projects/temperature-measurement/thermocouple/calibration-table
*/

double MAX31855::correctedTempCelsius(void)
{

    double correctedTemp;
    double deltaV;
    double wrongTemp = readCelsius();
    double To, Vo, p1, p2, p3, p4, q1, q2, q3;
    double thermocoupleVoltage;


    // Switch statement that depends on the type of thermocouple used. Coefficients are dependent on which thermocouple is used.
    switch (_type)
    {
    case 'T':
      if(thermocoupleVoltage <= -4.648){
          To = -1.9243000E+02;
          Vo = -5.4798963E+00;
          p1 = 5.9572141E+01;
          p2 = 1.9675733E+00;
          p3 = -7.8176011E+01;
          p4 = -1.0963280E+01;
          q1 = 2.7498092E-01;
          q2 = -1.3768944E+00;
          q3 = -4.5209805E-01;
      }

      else if(thermocoupleVoltage <= 0 && thermocoupleVoltage > -4.648){
          To = -6.0000000E+01;
          Vo = -2.1528350E+00;
          p1 = 3.0449332E+01;
          p2 = -1.2946560E+00;
          p3 = -3.0500735E+00;
          p4 = -1.9226856E-01;
          q1 = 6.9877863E-03;
          q2 = -1.0596207E-01;
          q3 = -1.0774995E-02;
      }

      else if(thermocoupleVoltage <= 9.288 && thermocoupleVoltage > 0){
          To = 1.3500000E+02;
          Vo = 5.9588600E+00;
          p1 = 2.0325591E+01;
          p2 = 3.3013079E+00;
          p3 = 1.2638462E-01;
          p4 = -8.2883695E-04;
          q1 = 1.7595577E-01;
          q2 = 7.9740521E-03;
          q3 = 0.0000000E+00;
      }
          else{
          To = 3.0000000E+02;
          Vo = 1.4861780E+01;
          p1 = 1.7214707E+01;
          p2 = -9.3862713E-01;
          p3 = -7.3509066E-02;
          p4 = 2.9576140E-04;
          q1 = -4.8095795E-02;
          q2 = -4.7352054E-03;
          q3 = 0.0000000E+00;
      }
    break;

    case 'K':
      thermocoupleVoltage = wrongTemp * 0.041276;

      if(thermocoupleVoltage <= -3.554){
          To = -1.2147164E+02;
          Vo = -4.1790858E+00;
          p1 = 3.6069513E+01;
          p2 = 3.0722076E+01;
          p3 = 7.7913860E+00;
          p4 = 5.2593991E-01;
          q1 = 9.3939547E-01;
          q2 = 2.7791285E-01;
          q3 = 2.5163349E-02;
      }

      else if(thermocoupleVoltage <= 4.096 && thermocoupleVoltage > -3.554){
          To = -8.7935962E+00;
          Vo = -3.4489914E-01;
          p1 = 2.5678719E+01;
          p2 = -4.9887904E-01;
          p3 = -4.4705222E-01;
          p4 = -4.4869203E-02;
          q1 = 2.3893439E-04;
          q2 = -2.0397750E-02;
          q3 = -1.8424107E-03;
      }

      else if(thermocoupleVoltage <= 16.397 && thermocoupleVoltage > 4.096){
          To = 3.1018976E+02;
          Vo = 1.2631386E+01;
          p1 = 2.4061949E+01;
          p2 = 4.0158622E+00;
          p3 = 2.6853917E-01;
          p4 = -9.7188544E-03;
          q1 = 1.6995872E-01;
          q2 = 1.1413069E-02;
          q3 = -3.9275155E-04;
      }
      else if(thermocoupleVoltage <= 33.275 && thermocoupleVoltage > 16.397){
          To = 6.0572562E+02;
          Vo = 2.5148718E+01;
          p1 = 2.3539401E+01;
          p2 = 4.6547228E-02;
          p3 = 1.3444400E-02;
          p4 = 5.9236853E-04;
          q1 = 8.3445513E-04;
          q2 = 4.6121445E-04;
          q3 = 2.5488122E-05;
      }
          else{
          To = 1.0184705E+03;
          Vo = 4.1993851E+01;
          p1 = 2.5783239E+01;
          p2 = -1.8363403E+00;
          p3 = 5.6176662E-02;
          p4 = 1.8532400E-04;
          q1 = -7.4803355E-02;
          q2 = 2.3841860E-03;
          q3 = 0.0000000E+00;
      }

      break;

    default:
      break;

    deltaV = thermocoupleVoltage - Vo;
    correctedTemp = To + (deltaV * (p1 + deltaV * (p2 + deltaV * (p3 + deltaV * p4)))) / (1 + deltaV * (q1 + deltaV * (q2 + deltaV * q3)));
    }

  return correctedTemp;
}

/*
Function to read errors from the thermocouples.
*/

String MAX31855::readError()
{
  unsigned long bits = readBits();
  String fault;
  if (bits & 0x00010000) // check if fault bit set
  {
    // switch statement that checks last three bits values to determine fault.
    switch (bits & 0x00000007)
    {
      case 0x01:
        fault = "No Thermocouple Connected";
        break;

      case 0x02:
        fault = "Thermocouple Shorted to GND";
        break;

      case 0x03:
        fault = "Thermocouple Shorted to VCC";
        break;

      default:
      fault = "No faults detected.";
      break;
    }
  }
  return fault;
}

/*
Function to read in the 32-bit output from the MAX31855.

It uses bitbanging to go through the bits, so they can be preserved for other operations.
*/

unsigned long MAX31855::readBits()
{
  // Instantiate an unsigned long for our raw data from MAX31855. Note, use the unsigned long so we can transfer bits directly.
  unsigned long rawBits = 0;
  digitalWrite(_cs, LOW); // Select chip (stop measuring and start communicating with Arduino)


  for (int i = 31; i >= 0; i--) // Iterator to go through each bit aka bitbanging
  {
    digitalWrite(_sclk, HIGH); // data sent after rising edge

    if (digitalRead(_miso)) // check if MISO bit is high
    {
      rawBits |= ((unsigned long)1 << i); // read if MISO bit is high, if so put a one in rawBits and bit shift left by iterator.
    }

    digitalWrite(_sclk, LOW); //data transfer complete on falling edge
  }

  digitalWrite(_cs, HIGH); // back to measuring

  return(rawBits);
}


// Instantiate Low Pass Filter
LowPassFilter::LowPassFilter(double alpha)
  : yPrev(0.0f)
{
  _alpha = alpha;
}


/*
Function to filter thermocouple data using a weighted moving average.
*/
double LowPassFilter::filter(double x)
{
  double y = _alpha * yPrev + (1.0f - _alpha) * x;
  yPrev = y;
  return y;
}
