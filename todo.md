# ADS1X15 library



## Support comparator functionality

// getters...4x
setComparatorMode(comp_mode);   // 0 = TRADITIONAL,  else = WINDOW  
setComparatorPolarity(mode);    // 0 = LOW           else = HIGH
setComparatorLatch(mode);       // 0 = NON LATCH     else = LATCH
setComperatorQueConvert(mode)   // 0..3

uint8_t  _compMode = 0;
uint8_t  _compPolarity = 0;
uint8_t  _compLatchMode = 0;
uint8_t  _compQueConvert = 0;

setCOmparatorThresholdLow(uint16_t low);
setCOmparatorThresholdHigh(uint16_t high);
uint16_t getCOmparatorThresholdLow();
uint16_t getCOmparatorThresholdHigh();


## Improve continuous mode (now it behaves too much like single mode)

test & write demo sketch
- readADC(pin) followed by getLastValue()
- rename getLatestValue();


## Improve documentation

always

## Support for Alert/rdy pin

yes

## 1013/1113 cannot set gain

- fix somehow (not a real showstopper but need at least documentation.
- setGain() could check type....
- 

## Testing
- getVoltage
- performance sketch()
- continuous sketch()

