# ADS1X15

Arduino library for I2C ADC ADS1015, ADS1115,

## Description

This library should work for the devices mentioned below,
although not all sensors support all functionality.

| Device | Channels | Resolution | Max sps | Comparator | ProgGainAMP | Notes |
|:----:|:----:|:----:|:----:|:----:|:----:|:----|
| ADS1013 | 1 | 12 | 3300 | N  | N |        |
| ADS1014 | 1 | 12 | 3300 | Y  | Y |        |
| ADS1015 | 4 | 12 | 3300 | Y  | Y |        |
| ADS1113 | 1 | 16 | 860  | N  | N |        |
| ADS1114 | 1 | 16 | 860  | Y  | Y |        |
| ADS1115 | 4 | 16 | 860  | Y  | Y | Tested |

As the 1015 and the 1115 are both 4 channels these are the most
interesting from functionality point of view as these can also do
differential measurement.

Note: The RDY pin is not confirmed working as it should.

#### Note

This readme file is work in progress.
Need a serious refactor too.


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


#### Programmable Gain

- **setGain(gain)** set the gain value, indicating the maxVoltage that can be measured
Adjusting the gain allows one to make more precise measurements.
See table below.
- **getGain()** returns the gain value.
- **getMaxVoltage()** returns the max voltage with the current gain.
- **toVoltage(raw = 1)** converts a raw measurement to a voltage.
Can be used for normal and differential measurements.
The default value of 1 returns the conversion factor for any raw number.

That factor can be used to set HIGH and LOW threshold registers in the comparison mode.
Check the examples.

| PGA value | Max Voltage | note |
|:----:|:----:|:----:|
| 0  | ±6.144V | default |
| 1  | ±4.096V |         |
| 2  | ±2.048V |         |
| 4  | ±1.024V |         |
| 8  | ±0.512V |         |
| 16 | ±0.256V |         |


#### Operational mode

- **setMode(mode)** 0 = CONTINUOUS, 1 = SINGLE (default)
- **getMode()** returns current mode


#### Datarate

- **setDataRate(dataRate)** Datarate depends on type of device.
For all devices the index 0..7 can be used, see table below.
Values above 7 ==> will be set to 4.
- **getDataRate()** returns the current datarate (index).

Datarate in samples per second.

| datarate | ADS101x | ADS 111x | Notes & remarks |
|:----:|----:|----:|:----:|
| 0 | 128  | 8   |  |
| 1 | 250  | 16  |  |
| 2 | 490  | 32  |  |
| 3 | 920  | 64  |  |
| 4 | 1600 | 128 | default |
| 5 | 2400 | 250 |  |
| 6 | 3300 | 475 |  |
| 7 | 3300 | 860 |  |


#### ReadADC Single mode

Reading the ADC is very straightforward, the **readADC()** function handles
all in one call. Under the hood it uses the asynchronuous calls.
- **int16_t readADC(pin)** normal ADC functionality, pin = 0..3

To read the ADC in an asynchronuous way one has to use three calls:
- **requestADC(pin)**  Start the conversion
- **isBusy()**  Is the conversion ready or not?
- **getLastValue()** Read the result of the conversion.


in terms of code
```cpp

  void setup()
  {
    // other setup things here
    ADS.requestADC(pin);
  }


  void loop()
  {
    if (!ADS.isBusy())
    {
      val = ADS.getLastValue();
      ADS.requestADC(pin);
    }
    // do other things here
  }

```
See example

## ReadADC Differential

For reading the ADC there are 4 calls possible.
- **readADC_Differential_0_1()** returns the difference between 2 ADC pins.
- **readADC_Differential_0_3()** ADS1x15 only
- **readADC_Differential_1_3()** ADS1x15 only
- **readADC_Differential_2_3()** ADS1x15 only

The differential reading of the ADC can also be done with asynchronuous calls.
- **requestADC_Differential_0_1()** starts conversion for differential reading
- **requestADC_Differential_0_3()** ADS1x15 only
- **requestADC_Differential_1_3()** ADS1x15 only
- **requestADC_Differential_2_3()** ADS1x15 only

After one of these calls one need to call
- **isBusy()**  Is the conversion ready?
- **getLastValue()** Read the result of the conversion.


#### ReadADC continuous mode

TO set the continuous mode one need two calls
- **setMode(0)** 0 = CONTINUOUS, 1 = SINGLE (default)
- **getLastValue()** returns last value read by the device.
Calling this over and over again can give the same value multiple times.
By using **isBusy()** one can wait until new data is available.

see example.


#### Threshold registers ==> mode RDY pin

If the thresholdHigh is set to 0x0100 and the thresholdLow to 0x0000
the **ALERT/RDY** pin is triggered when a conversion is ready.
- **setComparatorThresholdLow(0x0000)**
- **setComparatorThresholdHigh(0x0100)**

see examples


## Comparator

Please read Page 15 of the datasheet as the behavior of the
comparator is not trivial.


#### Comparator Mode

When configured as a **TRADITIONAL comparator**, the **ALERT/RDY** pin asserts
(active low by default) when conversion data exceed the limit set in the
high threshold register. The comparator then deasserts when the input
signal falls below the low threshold register value.

If the comparator **latch** is set, the **ALERT/RDY** pin asserts and it will be
reset after reading the sensor (conversion register) again.
*An SMB alert command (00011001) on the I2C bus will also reset the alert state.*
*Not implemented in the library (yet)*

In **WINDOW comparator** mode, the **ALERT/RDY** pin asserts if conversion data exceeds
the high threshold register or falls below the low threshold register.
In this mode the alert is held if the latch is set. This is similar as above.

The **ComperatorQueConvert** can be set to 0..3 see table below.
All other values will use 3 (disable comparator).

| value | meaning | notes |
|:----:|:----|:----|
| 0 | trigger alert after 1 conversion  | |
| 1 | trigger alert after 2 conversions | |
| 2 | trigegr alert after 4 conversions | |
| 3 | Disable comparator | default |


#### Polarity

Default alert is **LOW**, can be to set **HIGH**.


#### Latch

Holds the **ALERT/RDY** to **HIGH** (or **LOW** depending on polarity) after triggered
even if actual value has been 'restored to normal' value.

#### QueConvert

Set the number of conversions before trigger activates.
- **setComperatorQueConvert(uint8_t mode)** is used to set the number of
conversions that exceed the threshold before the **ALERT/RDY** pin is set **HIGH**.
A value of 3 (or above) disables the comparator.


#### Threshold registers comparator mode

Depending on the comparator mode TRADITIONAL or WINDOW the thresholds mean
something different see - Comparator Mode above.
- **setComparatorThresholdLow(lo)** set the low threshold; take care the hi >= lo
- **setComparatorThresholdHigh(hi)** set the high threshold; take care the hi >= lo
- **getComparatorThresholdLow()** returns set value
- **getComparatorThresholdHigh()** returns set value


## Future ideas & improvements

- Improve documentation
- examples?
- SMB alert command (00011001) on I2C bus?

## Operation

See examples
