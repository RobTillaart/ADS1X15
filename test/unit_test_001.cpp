//
//    FILE: unit_test_001.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
//    DATE: 2020-12-03
// PURPOSE: unit tests for the SHT31 temperature and humidity sensor
//          https://github.com/RobTillaart/ADS1X15
//

// supported assertions
// ----------------------------
// assertEqual(expected, actual)
// assertNotEqual(expected, actual)
// assertLess(expected, actual)
// assertMore(expected, actual)
// assertLessOrEqual(expected, actual)
// assertMoreOrEqual(expected, actual)
// assertTrue(actual)
// assertFalse(actual)
// assertNull(actual)

#include <ArduinoUnitTests.h>

#include "Arduino.h"
#include "ADS1X15.h"


unittest_setup()
{
}

unittest_teardown()
{
}

unittest(test_begin)
{
  ADS1115 ADS(0x48);
  bool b = ADS.begin();
  assertTrue(b);

  b = ADS.isConnected();
  assertFalse(b);
}

unittest(test_gain)
{
  ADS1115 ADS(0x48);
  bool b = ADS.begin();
  assertTrue(b);

  int gain = ADS.getGain();
  assertEqual(0, gain);
  int gains[6] = { 0,1,2,4,8,16 };
  for (int i = 0; i < 6; i++)
  {
    ADS.setGain(gains[i]);
    assertEqual(gains[i], ADS.getGain());
  }

  ADS.setGain(42);
  assertEqual(255, ADS.getGain());
}


unittest_main()

// --------
