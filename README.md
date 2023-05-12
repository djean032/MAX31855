# MAX31855

## Project Description
A solution to control and monitor four thermocouples using the MAX31855 Thermocouple Amplifier and an Arduino Micro.

## Contents
- MAX31855 library
- Features
- KICAD Generated Schematic and PCB
- Evaluation
- v2

## MAX31855 Library

Custom library that creates a class instance of each thermocouple amplifier and provides functions to read data, check errors, and do a rational polynomial caluclation to accurately determine the temperature. Additionally a weighted average is used to filter the signal, resulting in a significantly smoothed reading.

The initial form of this library uses a switch statement to select the proper conversion function based on thermocouple type, of which only type 'T' and 'K' are currently supported. Additional types will be added in the future, so this library can be universally used across all thermocouple and MAX31855 types.

## Features

#### Overview

The circuit consists of four MAX31855 thermocouple amplifiers that are connected to an Arduino Micro. The Arduino Micro is connected to the the thermocouple amplifiers via SPI, utilizing the SCK, MISO, and CS lines. The Arduino also communicates the data to a PC via serial while also displaying it on an onboard LCD.

#### MAX31855 Idiosyncrasies

The MAX31855 transmits a cold-junction compensated thermocouple temperature, a cold-junction temperature (from a thermistor), and several fault checks over 32 bits. The first 14 bits are the cold-junction compensated thermocouple temperature, the next 12 bits are the cold-junction temperature, and the rest of the bits are either reserved bits or denote various fault checks.

The MAX31855 uses a linear conversion to convert the thermocouple voltage to a temperature. Specifically the type 'T' version uses a factor of $52.18 \frac{\mu V}{\textdegree C}$, which shows significant deviations at room temperature from tabulated values. For this application, this is not a suitable conversion method, so a rational polynomial function was employed in software to more accurately  model the thermocouple response. The basic steps are to convert the measured thermocouple temperature to a voltage using the linear relationship stated in the datasheet. Then a switch statement is used to slect the type of chip to determine the appropriate coefficients for the rational polynomial function. Finally, the thermocouple voltage and coefficients are used in the rational polynomial function to compute a corrected temperature. This was tested at several temperatures and provides an acceptable level of accuracy.

#### Data Transfer

The data is simultaneously displayed on an LCD screen and transmitted to a PC via USB, where it is read into our lab's LabView program used to control our spectrometer. The data is "filtered" using a weighted average that takes a weighted average of the previous value with the current value. This weight can be adjusted via software. 30% of the previous value seems to be a sweet spot between data smoothing and quick response time.

#### Power Supply

The device is powered via a 9V external AC to DC power supply. In our current application, we have an approximately 15m USB cable, which is triple the spec for USB 2.0. This introduces quite a bit of noise into the system; therefore, an internal extension  "data-only" cable has been added to prevent this power line noise. A USB-B to USB-B "isolator" that only connects the GND and data lines will be added in the future to prevent modifications of the Arduino itself or the need for a custom cable.

#### Miscellaneous

The MAX31855 requires 3.3V to operate, while the Arduino Micro operates at 5V, so two bi-directional level shifters were used for the data and power lines for the thermocouple amplifiers.

## Schematic
![image](https://user-images.githubusercontent.com/59145040/236655637-3a5744ae-9aae-4677-a55e-aa6a550ce21e.png)

## PCB
![image](https://user-images.githubusercontent.com/59145040/236655648-e1c19021-bc43-46e3-9a99-b2080be4efc2.png)


## Evaluation

Overall, once the rational polynomial function and filtering was added, the performance was adequate for our lab's needs. It successfully reads temperatures from room temperature down to cryogenic temperatures used in our lab's spectrometer.

Further improvements will be implemented in v2. These include capacitors to clean up the power supply and capacitors added on to the VCC and GND lines of the thermocouple amplifiers to prevent coupling of the data and power supply lines.


## v2
- Proper power supply design
	- Add appropriate capacitors to the input to reduce noise
- Add small capacitors to the VCC and GND lines of the thermocouple amplifiers to prevent coupling of the signal and the power lines.
