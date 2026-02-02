//
//    FILE: ADS_meta_info.ino
//  AUTHOR: Rob.Tillaart
// PURPOSE: read analog inputs - straightforward.
//     URL: https://github.com/RobTillaart/ADS1X15


#include "ADS1X15.h"

ADS1115 ADS(0x48);


void setup() 
{
  Serial.begin(115200);
  Serial.println();
  Serial.println(__FILE__);
  Serial.print("ADS1X15_LIB_VERSION: ");
  Serial.println(ADS1X15_LIB_VERSION);
  Serial.println();

  Serial.print(ADS.LibName());
  Serial.print(": ");
  Serial.println(ADS.LibVersion());
  Serial.println(ADS.LibURL());
  Serial.println(ADS.LibAuthor());
}


void loop() 
{
}


//  -- END OF FILE --
