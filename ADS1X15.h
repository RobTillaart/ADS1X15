#pragma once
//
//    FILE: ADS1X15.H
//  AUTHOR: Rob Tillaart
// VERSION: 0.2.2
//    DATE: 2013-03-24
// PUPROSE: Arduino library for ADS1015 and ADS1115
//     URL: https://github.com/RobTillaart/ADS1X15
//

#include "Arduino.h"
#include "Wire.h"

#define ADS1X15_LIB_VERSION               "0.2.2"

// allow compile time default address
// address in { 0x48, 0x49, 0x4A, 0x4B }, no test...
#ifndef ADS1015_ADDRESS
#define ADS1015_ADDRESS             0x48
#endif

#ifndef ADS1115_ADDRESS
#define ADS1115_ADDRESS             0x48
#endif


#define ADS1X15_INVALID_VOLTAGE     -100
#define ADS1X15_INVALID_GAIN        0xFF


class ADS1X15
{
public:
#if defined (ESP8266) || defined(ESP32)
  void begin(uint8_t sda, uint8_t scl);
#endif
  bool     begin();
  bool     isBusy();
  bool     isConnected();

  //       GAIN
  // 0  =  ±6.144V  default
  // 1  =  ±4.096V
  // 2  =  ±2.048V
  // 4  =  ±1.024V
  // 8  =  ±0.512V
  // 16 =  ±0.256V
  void     setGain(uint8_t gain = 0);
  uint8_t  getGain();                    //  0xFF == invalid gain error.

  // both may return ADS1X15_INVALID_VOLTAGE if the gain is invalid.
  float    toVoltage(int16_t val = 1);   //  converts raw to voltage
  float    getMaxVoltage();              //  -100 == invalid gain error

  // 0  =  CONTINUOUS
  // 1  =  SINGLE      default
  void     setMode(uint8_t mode = 1);
  uint8_t  getMode();                 //  0xFF == invalid mode error.
  
  // 0 = slowest   7 == fastest  4 = default
  void     setDataRate(uint8_t dataRate); 
  uint8_t  getDataRate();  // actual speed depends on device
  
  int16_t  readADC(uint8_t pin);
  int16_t  readADC_Differential_0_1();

  int16_t  getLastValue();

protected:
  ADS1X15();

  // CONFIGURATION
  // BIT  DESCRIPTION
  // 0    # channels        0 == 1    1 == 4;
  // 1    0
  // 2    # resolution      0 == 12   1 == 16
  // 3    0
  // 4    has gain          0 = NO    1 = YES
  // 5    has comparator    0 = NO    1 = YES
  // 6    0
  // 7    0
  uint8_t  _config;
  uint8_t  _maxPorts;
  uint8_t  _address;
  uint8_t  _conversionDelay;
  uint8_t  _bitShift;
  uint16_t _gain;
  uint16_t _mode;
  uint16_t _datarate;

  int16_t _readADC(uint16_t readmode);
};

///////////////////////////////////////////////////////////////////////////
//
// Derived classes from ADS1X15
//
class ADS1013 : public ADS1X15
{
public:
  ADS1013(uint8_t Address = ADS1015_ADDRESS);
};

class ADS1014 : public ADS1X15
{
public:
  ADS1014(uint8_t Address = ADS1015_ADDRESS);
};

class ADS1015 : public ADS1X15
{
public:
  ADS1015(uint8_t Address = ADS1015_ADDRESS);
  int16_t  readADC_Differential_0_3();
  int16_t  readADC_Differential_1_3();
  int16_t  readADC_Differential_2_3();
};

class ADS1113 : public ADS1X15
{
public:
  ADS1113(uint8_t address = ADS1115_ADDRESS);
};

class ADS1114 : public ADS1X15
{
public:
  ADS1114(uint8_t address = ADS1115_ADDRESS);
};

class ADS1115 : public ADS1X15
{
public:
  ADS1115(uint8_t address = ADS1115_ADDRESS);
  int16_t  readADC_Differential_0_3();
  int16_t  readADC_Differential_1_3();
  int16_t  readADC_Differential_2_3();
};

// --- END OF FILE ---
