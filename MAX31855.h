/*
MAX31855.h - Library for use with the MAX31855 Thermocouple amplifer.

Created by Dairen Jean 6MAY23.

Initial Support for Type T and K Thermocouples added on 6MAY23.
*/

#ifndef MAX31855_h
#define MAX31855_h

#include "Arduino.h"

// Thermocouple amplifier class for MAX 31855
class MAX31855 {

  public:
    MAX31855(int8_t sclk, int8_t cs, int8_t miso, char type);
    bool begin(void);
    double readInternal(void);
    double readCelsius(void);
    double correctedTempCelsius(void);
    String readError(void);
    unsigned long readBits(void);

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
