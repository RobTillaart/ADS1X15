# ADS1X15

Arduino library for I2C ADC ADS1015, ADS1115,

## Description

This library should work for the devices mentioned below, 
although not all sensors support all functionality. 

| Device | Channels | Resolution | Max sps | Comparator | ProgGainAMP | Notes |
|:----:|:----:|:----:|:----:|:----:|:----:|:----|
| ADS1013 | 1 | 12 | 860  | N  | N |        |
| ADS1014 | 1 | 12 | 860  | Y  | Y |        |
| ADS1015 | 4 | 12 | 860  | Y  | Y |        |
| ADS1113 | 1 | 16 | 3300 | N  | N |        |
| ADS1114 | 1 | 16 | 3300 | Y  | Y |        |
| ADS1115 | 4 | 16 | 3300 | Y  | Y | Tested |

As the 1015 and the 1115 are both 4 channels these are the most 
interesting from functionality point of view as these can also do
differential measurement.

Functionality not implemented yet is the **comparator** functionality.
These need some hands on experience first.


## Interface

The address of the ADS1113/4/5 is determined by to which pin the ADDR
is connected to:

| ADDR pin connected to | Address | Note |
|:----:|:----:|:----:|
| GND | 0x48 | default |
| VDD | 0x49 |
| SDA | 0x4A |
| SCL | 0x4B |

- **ADS1x15()** constructor, should not be used.
- **ADS1013(address)** constructor
- **ADS1014(address)** constructor
- **ADS1015(address)** constructor
- **ADS1113(address)** constructor
- **ADS1114(address)** constructor
- **ADS1115(address)** constructor

### Programmable Gain

- **setGain(gain)** set the gain value, indicating the maxVoltage that can be measured  
Adjusting the gain allows one to make more precise measurements.  
See table below.
- **getGain()** returns the gain value.
- **getMaxVoltage()** returns the max voltage with the current gain.
- **toVoltage(raw = 1)** converts a raw measurement to a voltage.  
Can be used for normal and differential measurements.  
The default value of 1 returns the conversion factor for any raw number.

| PGA value | Max Voltage | note |
|:----:|:----:|:----:|
| 0  | ±6.144V | default |
| 1  | ±4.096V |         |
| 2  | ±2.048V |         |
| 4  | ±1.024V |         |
| 8  | ±0.512V |         |
| 16 | ±0.256V |         |


### Operational mode

- **setMode(mode)** 0 = CONTINUOUS, 1 = SINGLE (default)
- **getMode()** returns current mode

### Datarate

- **setDataRate(dataRate)** Datarate depends on type of device.  
For all devices the index 0..7 can be used, see table below.
- **getDataRate()** returns the current datarate (index).

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

### ReadADC 

- **readADC(pin)** normal ADC functionality, pin = 

### ReadADC Differential

- **readADC_Differential_0_1()** returns the difference between 2 ADC pins.
- **readADC_Differential_0_3()** 1x15 only
- **readADC_Differential_1_3()** 1x15 only
- **readADC_Differential_2_3()** 1x15 only 

### Misc.

- **getLastValue()** returns last value read by the device.


## ALERT/RDY pin

Also known as Conversion Ready pin, is not supported in this library.

## Future

- Support comparator functionality
- Improve documentation
- Support for Alert/rdy pin
- 1013/1113 cannot set gain (bug)


## Operation

See examples
