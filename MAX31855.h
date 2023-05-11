/*
MAX31855.h - Library for use with the MAX31855 Thermocouple amplifer.

Created by Dairen Jean 6MAY23.

Initial Support for Type T and K Thermocouples added on 6MAY23.
*/

#ifndef MAX31855_h
#define MAX31855_h

#include "Arduino.h"
#include "SPI.h"

// Thermocouple amplifier class for MAX 31855
class MAX31855 {

  public:
    MAX31855(int8_t sclk, int8_t cs, int8_t miso, char type);
    double readInternal(int32_t bits1);
    double readCelsius(int32_t bits2);
    double correctedTempCelsius(int32_t bits);
    String readError(int32_t bits3);
    int32_t readBits(void);
    void MAX31855::begin(void);

  private:
    int8_t _sclk;
    int8_t _cs;
    int8_t _miso;
    char _type;

};


// Linear Weighted Moving Average utilized as low pass filter. Alpha is the weight.
class LowPassFilter {
  public:
    LowPassFilter(double alpha);
    ~LowPassFilter() = default;

    double filter(double x);
    double _alpha;

  protected:
    double yPrev;
};

#endif
