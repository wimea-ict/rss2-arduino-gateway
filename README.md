# rss2-arduino-gateway
An implementation of WIMEA-ICT gateway functionality on the RSS2 board using the arduino language.
Radio functions are all included in RadioFunctions.h, including on-board LED definitions.
Functionality is: Receive --> Wake up MCU--> Append timestamp from RTC --> save to SD card.
The ATMEGA256RFR2 Radio Power Consumption Control is also implemented in a function called RPCEnable().
Chip Select pin is PD6/OW2 for temperature sensor.
Please add "RnC" folder  to the hardware folder of Arduino (on Windows, this is C:\Users\user-name-here\Documents\Arduino\hardware). This contains the Arduino Board definition file for ATMEGA256RFR2 Xplained Pro Board from RiddlenCode, slightly edited to port it to the RSS2 board.
