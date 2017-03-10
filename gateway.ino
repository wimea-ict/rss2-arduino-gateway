#include "RadioFunctions.h"
#include <ctype.h>
#include "Wire.h"
#include <SPI.h>
#include <SD.h>
#include <avr/sleep.h>
#include <avr/power.h>

#define DS3231_I2C_ADDRESS 0x68

byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
char c = '\0';
String data=""; 
const int chipSelect = 15; //ow2
File dataFile;


/*************************RTC FUNCTIONS**************************/
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}
byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}

void readRTCTime(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
void setRTCTime(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

String getTimeString()
{
  // retrieve data from DS3231
 readRTCTime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year); 
 String time = "";
 if(dayOfMonth<10) { time = time + "0"+String(dayOfMonth);} else {time = String(dayOfMonth);}
 if(month<10) { time = time + '/' + "0"+String(month);} else {time = time + '/' +String(month);}
time = time + '/' +String(year);
time = time + " ";
if(hour<10) { time = time + "0"+String(hour);} else {time = time +String(hour);}
if(minute<10) { time = time + ':' + "0"+String(minute);} else {time = time + ':' +String(minute);}
if(second<10) { time = time + ':' + "0"+String(second);} else {time = time + ':' +String(second);}
  return time;
}


/********************************MAIN**********************************/
void setup()
{
  Serial.begin(38400);  // Start up serial
  Wire.begin();
  rfBegin(26);  // Initialize ATmega128RFA1 radio on channel 26 
  RPCEnable();  //Transceiver Reduced Power Consumption Control
  data.reserve(200);  //200 bytes for data string
  pinMode(10, OUTPUT);      //pb0 PIN 10 hardware chipselect must be an output for sd functions to work 
  if (!SD.begin(chipSelect)) {
    return;
  }
}

void loop()
{
  while (rfAvailable())  // If data receieved on radio...
  {
      c=rfRead();
      if(isprint(c))
      {
        data +=c;
      }
          if((c==-1) & (data.indexOf("TXT")>-1))  //report read ended and valid report contains TXT
          {
             data = getTimeString() + " "+ data.substring(2) + " [RSSI=" + String(rssiRaw) + "]";
             dataFile = SD.open("data.txt", FILE_WRITE);
                // if the file is available, write to it:
              if (dataFile) 
               {
                 dataFile.println(data);
                 data="";                 
               }
           dataFile.close();
            break;
            }
    }
       set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode.
       sleep_enable(); // Enable sleep mode.
       sleep_mode(); // Enter sleep mode.
      // After waking from watchdog interrupt the code continues to execute from this point.
       sleep_disable(); // Disable sleep mode after waking.    
}
