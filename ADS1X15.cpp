//
//    FILE: ADS1X15.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.2.3
//    DATE: 2013-03-24
// PUPROSE: Arduino library for ADS1015 and ADS1115
//     URL: https://github.com/RobTillaart/ADS1X15
//
// HISTORY:
// 0.0.0   2013-03-24 initial version
// 0.0.1   2013-03-24 first working version
// 0.1.0   2017-07-31 removed pre 1.0 support; added getVoltage
// 0.2.0   2020-04-08 initial release; refactor ad fundum;
// 0.2.1   2020-08-15 fix issue 2 gain; refactor
// 0.2.2   2020-08-18 add begin(sda, scl) for ESP32
// 0.2.3   2020-08-20 add comparator code + async mode

#include "ADS1X15.h"

#define ADS1015_CONVERSION_DELAY    1
#define ADS1115_CONVERSION_DELAY    8


// Kept #defines a bit in line with Adafruits library.

// REGISTERS
#define ADS1X15_REG_CONVERT         0x00
#define ADS1X15_REG_CONFIG          0x01
#define ADS1X15_REG_LOW_THRESHOLD   0x02
#define ADS1X15_REG_HIGH_THRESHOLD  0x03


// CONFIG REGISTER

// BIT 15       Operational Status         // 1 << 15
#define ADS1X15_OS_BUSY             0x0000
#define ADS1X15_OS_NOT_BUSY         0x8000
#define ADS1X15_OS_START_SINGLE     0x8000

// BIT 12-14    read differential
#define ADS1X15_MUX_DIFF_0_1        0x0000
#define ADS1X15_MUX_DIFF_0_3        0x1000
#define ADS1X15_MUX_DIFF_1_3        0x2000
#define ADS1X15_MUX_DIFF_2_3        0x3000
// read single
#define ADS1X15_READ_0              0x4000
#define ADS1X15_READ_1              0x5000
#define ADS1X15_READ_2              0x6000
#define ADS1X15_READ_3              0x7000


// BIT 9-11     gain                        // (0..5) << 9
#define ADS1X15_PGA_6_144V          0x0000  // voltage
#define ADS1X15_PGA_4_096V          0x0200  //
#define ADS1X15_PGA_2_048V          0x0400  // default
#define ADS1X15_PGA_1_024V          0x0600
#define ADS1X15_PGA_0_512V          0x0800
#define ADS1X15_PGA_0_256V          0x0A00

// BIT 8        mode                        // 1 << 8
#define ADS1X15_MODE_CONTINUE       0x0000
#define ADS1X15_MODE_SINGLE         0x0100

// BIT 5-7      datarate sample per second  // (0..7) << 5
/*
differs for different devices, check datasheet or readme.md
| datarate | ADS101x | ADS 111x |
|:----:|----:|----:|
| 0 | 128  | 8   |
| 1 | 250  | 16  |
| 2 | 490  | 32  |
| 3 | 920  | 64  |
| 4 | 1600 | 128 |
| 5 | 2400 | 250 |
| 6 | 3300 | 475 |
| 7 | 3300 | 860 |
*/

/*
 * COMPARATOR MODE IS NOT IMPLEMENTED YET
 */

// BIT 4 comparator modi                    // 1 << 4
#define ADS1X15_COMP_MODE_TRADITIONAL   0x0000
#define ADS1X15_COMP_MODE_WINDOW        0x0010

// BIT 3 ALERT active value                 // 1 << 3
#define ADS1X15_COMP_POL_ACTIV_LOW      0x0000
#define ADS1X15_COMP_POL_ACTIV_HIGH     0x0008

// BIT 2 ALERT latching                     // 1 << 2
#define ADS1X15_COMP_NON_LATCH          0x0000
#define ADS1X15_COMP_LATCH              0x0004

// BIT 0-1 ALERT mode                       // (0..3)
#define ADS1X15_COMP_QUE_1_CONV         0x0000
#define ADS1X15_COMP_QUE_2_CONV         0x0001
#define ADS1X15_COMP_QUE_4_CONV         0x0002
#define ADS1X15_COMP_QUE_NONE           0x0003

// _CONFIG masks
#define ADS_CONF_CHAN_1  0x00
#define ADS_CONF_CHAN_4  0x01
#define ADS_CONF_RES_12  0x00
#define ADS_CONF_RES_16  0x04
#define ADS_CONF_GAIN    0x10
#define ADS_CONF_COMP    0x20          // TODO restrict calls.


/////////////////////////////////////////////////////////////////////////


static void writeRegister(uint8_t address, uint8_t reg, uint16_t value)
{
  Wire.beginTransmission(address);
  Wire.write((uint8_t)reg);
  Wire.write((uint8_t)(value >> 8));
  Wire.write((uint8_t)(value & 0xFF));
  Wire.endTransmission();
}

static uint16_t readRegister(uint8_t address, uint8_t reg)
{
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(address, (uint8_t) 2);
  uint16_t value = Wire.read() << 8;
  value += Wire.read();
  return value;
}

//
// CONSTRUCTOR
//
ADS1X15::ADS1X15()
{
  setGain(0);      // _gain = ADS1X15_PGA_6_144V;
  setMode(1);      // _mode = ADS1X15_MODE_SINGLE;
  setDataRate(4);  // middle speed, depends on device.
}

//
// PUBLIC
//
#if defined (ESP8266) || defined(ESP32)
bool ADS1X15::begin(uint8_t sda, uint8_t scl)
{
  Wire.begin(sda, scl);
  if (_address < 0x48 || _address > 0x4B) return false;
  return true;
}
#endif

bool ADS1X15::begin()
{
  Wire.begin();
  if (_address < 0x48 || _address > 0x4B) return false;
  return true;
}

bool ADS1X15::isBusy()
{
  uint16_t val = readRegister(_address, ADS1X15_REG_CONFIG);
  if ((val & ADS1X15_OS_NOT_BUSY) > 0) return false;
  return true;
}

bool ADS1X15::isConnected()
{
  Wire.beginTransmission(_address);
  return Wire.endTransmission() == 0;
}

void ADS1X15::setGain(uint8_t gain)
{
  if (!(_config & ADS_CONF_GAIN)) gain = 0;
  switch (gain)
  {
    default:  // catch invalid values and go for the safest gain.
    case 0:  _gain = ADS1X15_PGA_6_144V;  break;
    case 1:  _gain = ADS1X15_PGA_4_096V;  break;
    case 2:  _gain = ADS1X15_PGA_2_048V;  break;
    case 4:  _gain = ADS1X15_PGA_1_024V;  break;
    case 8:  _gain = ADS1X15_PGA_0_512V;  break;
    case 16: _gain = ADS1X15_PGA_0_256V;  break;
  }
}

uint8_t ADS1X15::getGain()
{
  if (!(_config & ADS_CONF_GAIN)) return 0;
  switch (_gain)
  {
    case ADS1X15_PGA_6_144V: return 0;
    case ADS1X15_PGA_4_096V: return 1;
    case ADS1X15_PGA_2_048V: return 2;
    case ADS1X15_PGA_1_024V: return 4;
    case ADS1X15_PGA_0_512V: return 8;
    case ADS1X15_PGA_0_256V: return 16;
  }
  return ADS1X15_INVALID_GAIN;
}

float ADS1X15::toVoltage(int16_t val)
{
  if (val == 0) return 0;

  float volts = getMaxVoltage();
  if (volts < 0) return volts;

  volts *= val;
  if (_config & ADS_CONF_RES_16)
  {
    volts /= 32767;  // val = 16 bits - signed
  }
  else
  {
    volts /= 2047;   // val = 12 bits - signed
  }
  return volts;
}

float ADS1X15::getMaxVoltage()
{
  switch (_gain)
  {
    case 0:  return 6.144;
    case 1:  return 4.096;
    case 2:  return 2.048;
    case 4:  return 1.024;
    case 8:  return 0.512;
    case 16: return 0.256;
  }
  return ADS1X15_INVALID_VOLTAGE;
}

void ADS1X15::setMode(uint8_t mode)
{
  switch (mode)
  {
    case 0: _mode = ADS1X15_MODE_CONTINUE; break;
    default:
    case 1: _mode = ADS1X15_MODE_SINGLE;   break;
  }
}

uint8_t ADS1X15::getMode(void)
{
  switch (_mode)
  {
    case ADS1X15_MODE_CONTINUE: return 0;
    case ADS1X15_MODE_SINGLE:   return 1;
  }
  return 0xFF;
}

void ADS1X15::setDataRate(uint8_t dataRate)
{
  _datarate = dataRate;
  if (_datarate > 7) _datarate = 4;  // default
  _datarate <<= 5;          // convert 0..7 to mask.
}

uint8_t ADS1X15::getDataRate(void)
{
  return (_datarate >> 5);  // convert mask back to 0..7
}

int16_t ADS1X15::_readADC(uint16_t readmode)
{
  _requestADC(readmode);
  if (_mode == ADS1X15_MODE_SINGLE)
  {
    while ( isBusy() ) yield();   // wait for conversion; yield for ESP.
  }
  else
  {
    delay(_conversionDelay);      // TODO CHECK if needed
  }
  return getLastValue();
}

int16_t ADS1X15::readADC(uint8_t pin)
{
  if (pin >= _maxPorts) return 0;
  uint16_t mode = ((4 + pin) << 12); // pin to mask
  return _readADC(mode);
}

void  ADS1X15::requestADC_Differential_0_1()
{
  _requestADC(ADS1X15_MUX_DIFF_0_1);
}

int16_t ADS1X15::readADC_Differential_0_1()
{
  return _readADC(ADS1X15_MUX_DIFF_0_1);
}

void ADS1X15::requestADC(uint8_t pin)
{
  if (pin >= _maxPorts) return;
  uint16_t mode = ((4 + pin) << 12);   // pin to mask
  _requestADC(mode);
}

void ADS1X15::_requestADC(uint16_t readmode)
{
  // write to register is needed in continuous mode as other flags can be changed
  uint16_t config = ADS1X15_OS_START_SINGLE;  // bit 15     force wake up if needed
  config |= readmode;                         // bit 12-14
  config |= _gain;                            // bit 9-11
  config |= _mode;                            // bit 8
  config |= _datarate;                        // bit 5-7
  if (_compMode)  config |= ADS1X15_COMP_MODE_WINDOW;         // bit 4      comparator modi
  else            config |= ADS1X15_COMP_MODE_TRADITIONAL;
  if (_compPol)   config |= ADS1X15_COMP_POL_ACTIV_HIGH;      // bit 3      ALERT active value
  else            config |= ADS1X15_COMP_POL_ACTIV_LOW;
  if (_compLatch) config |= ADS1X15_COMP_LATCH;
  else            config |= ADS1X15_COMP_NON_LATCH;           // bit 2      ALERT latching
  config |= _compQueConvert;                                  // bit 0..1   ALERT mode
  writeRegister(_address, ADS1X15_REG_CONFIG, config);
}

int16_t ADS1X15::getLastValue()
{
  int16_t raw = readRegister(_address, ADS1X15_REG_CONVERT);
  if (_bitShift) raw >>= _bitShift;  // Shift 12-bit results
  return raw;
}

void ADS1X15::setComparatorThresholdLow(int16_t lo)
{
  writeRegister(_address, ADS1X15_REG_LOW_THRESHOLD, lo);
};

int16_t ADS1X15::getComparatorThresholdLow()
{
  return readRegister(_address, ADS1X15_REG_LOW_THRESHOLD);
};

void ADS1X15::setComparatorThresholdHigh(int16_t hi)
{
  writeRegister(_address, ADS1X15_REG_HIGH_THRESHOLD, hi);
};

int16_t ADS1X15::getComparatorThresholdHigh()
{
  return readRegister(_address, ADS1X15_REG_HIGH_THRESHOLD);
};


///////////////////////////////////////////////////////////////////////////
//
// Derived classes from ADS1X15
//
ADS1013::ADS1013(uint8_t address)
{
  _address = address;
  _config = 0x00;
  _conversionDelay = ADS1015_CONVERSION_DELAY;
  _bitShift = 4;
  _maxPorts = 1;
}

ADS1014::ADS1014(uint8_t address)
{
  _address = address;
  _config = 0x30;
  _conversionDelay = ADS1015_CONVERSION_DELAY;
  _bitShift = 4;
  _maxPorts = 1;
}

ADS1015::ADS1015(uint8_t address)
{
  _address = address;
  _config = 0x34;
  _conversionDelay = ADS1015_CONVERSION_DELAY;
  _bitShift = 4;
  _maxPorts = 4;
}

int16_t ADS1015::readADC_Differential_0_3()
{
  return _readADC(ADS1X15_MUX_DIFF_0_3);
}

int16_t ADS1015::readADC_Differential_1_3()
{
  return _readADC(ADS1X15_MUX_DIFF_1_3);
}

int16_t ADS1015::readADC_Differential_2_3()
{
  return _readADC(ADS1X15_MUX_DIFF_2_3);
}

void ADS1015::requestADC_Differential_0_3()
{
  _requestADC(ADS1X15_MUX_DIFF_0_3);
}

void ADS1015::requestADC_Differential_1_3()
{
  _requestADC(ADS1X15_MUX_DIFF_1_3);
}

void ADS1015::requestADC_Differential_2_3()
{
  _requestADC(ADS1X15_MUX_DIFF_2_3);
}

///////////////////////////////////////////////////////////////////////////
//
// Derived classes from ADS1X15
//
ADS1113::ADS1113(uint8_t address)
{
  _address = address;
  _config = 0x01;
  _conversionDelay = ADS1115_CONVERSION_DELAY;
  _bitShift = 0;
  _maxPorts = 1;
}

ADS1114::ADS1114(uint8_t address)
{
  _address = address;
  _config = 0x31;
  _conversionDelay = ADS1115_CONVERSION_DELAY;
  _bitShift = 0;
  _maxPorts = 1;
}

ADS1115::ADS1115(uint8_t address)
{
  _address = address;
  _config = 0x35;
  _conversionDelay = ADS1115_CONVERSION_DELAY;
  _bitShift = 0;
  _maxPorts = 4;
}

int16_t ADS1115::readADC_Differential_0_3()
{
  return _readADC(ADS1X15_MUX_DIFF_0_3);
}

int16_t ADS1115::readADC_Differential_1_3()
{
  return _readADC(ADS1X15_MUX_DIFF_1_3);
}

int16_t ADS1115::readADC_Differential_2_3()
{
  return _readADC(ADS1X15_MUX_DIFF_2_3);
}

void ADS1115::requestADC_Differential_0_3()
{
  _requestADC(ADS1X15_MUX_DIFF_0_3);
}

void ADS1115::requestADC_Differential_1_3()
{
  _requestADC(ADS1X15_MUX_DIFF_1_3);
}

void ADS1115::requestADC_Differential_2_3()
{
  _requestADC(ADS1X15_MUX_DIFF_2_3);
}

// --- END OF FILE
