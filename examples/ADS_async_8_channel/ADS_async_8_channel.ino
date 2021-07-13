//
//    FILE: ADS_async_8_channel.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
// PURPOSE: demo reading two ADS1115 modules in parallel
//    DATE: 2021-07-05
//     URL: https://github.com/RobTillaart/ADS1X15


// Note all IO with the sensors are guarded by an isConnected()
// this is max robust, in non critical application one may either
// cache the value or only verify it in setup (least robust).
// Less robust may cause the application to hang - watchdog reset ?


#include "ADS1X15.h"


ADS1115 ADS0(0x48);
ADS1115 ADS1(0x49);
//ADS1115 ADS2(0x4A);
//ADS1115 ADS3(0x4B);

int16_t val0[4] = { 0, 0, 0, 0 };
int16_t val1[4] = { 0, 0, 0, 0 };
//int16_t val2[4] = { 0, 0, 0, 0 };
//int16_t val3[4] = { 0, 0, 0, 0 };
int     idx = 0;


void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("ADS1X15_LIB_VERSION: ");
  Serial.println(ADS1X15_LIB_VERSION);

  ADS0.begin();
  ADS1.begin();
  //  ADS2.begin();
  //  ADS3.begin();
  Serial.println(ADS0.isConnected());
  Serial.println(ADS1.isConnected());
  //  Serial.println(ADS2.isConnected());
  //  Serial.println(ADS3.isConnected());
  idx = 0;
  ADS_request_all();
}


void loop()
{
  Serial.println(__FUNCTION__);
  // wait until all is read...
  while (ADS_read_all());

  // we have all 8 values
  ADS_print_all();

  delay(1000);      // wait a second.
  ADS_request_all();
}


void ADS_request_all()
{
  // Serial.println(__FUNCTION__);
  if (ADS0.isConnected()) ADS0.requestADC(idx);
  if (ADS1.isConnected()) ADS1.requestADC(idx);
  //  if (ADS2.isConnected()) ADS2.requestADC(idx);
  //  if (ADS3.isConnected()) ADS3.requestADC(idx);
}


bool ADS_read_all()
{
  if (ADS0.isConnected() && ADS0.isBusy()) return true;
  if (ADS1.isConnected() && ADS1.isBusy()) return true;
  //  if (ADS2.isConnected() && ADS2.isBusy()) return true;
  //  if (ADS3.isConnected() && ADS3.isBusy()) return true;
  //Serial.print("IDX:\t");
  //Serial.println(idx);
  if (ADS0.isConnected()) val0[idx] = ADS0.getValue();
  if (ADS1.isConnected()) val1[idx] = ADS1.getValue();
  //  if (ADS2.isConnected()) val2[idx] = ADS2.getValue();
  //  if (ADS3.isConnected()) val3[idx] = ADS3.getValue();
  idx++;
  if (idx < 4)
  {
    ADS_request_all();
    return true;
  }
  idx = 0;
  return false;
}


void ADS_print_all()
{
  // Serial.println(__FUNCTION__);
  // TIMESTAMP
  Serial.println(millis());

  // PRINT ALL VALUES OF ADC0
  for (int i = 0; i < 4; i++)
  {
    Serial.print(val0[i]);
    Serial.print("\t");
  }
  // PRINT ALL VALUES OF ADC1
  for (int i = 0; i < 4; i++)
  {
    Serial.print(val1[i]);
    Serial.print("\t");
  }
  Serial.println();
  //  // PRINT ALL VALUES OF ADC2
  //  for (int i = 0; i < 4; i++)
  //  {
  //    Serial.print(val2[i]);
  //    Serial.print("\t");
  //  }
  //  // PRINT ALL VALUES OF ADC3
  //  for (int i = 0; i < 4; i++)
  //  {
  //    Serial.print(val3[i]);
  //    Serial.print("\t");
  //  }
  //  Serial.println();
}

// -- END OF FILE --
