/*
* Adapted by Josh Long (https://github.com/longjos) Oct 2015
* Based on a https://github.com/robinvanemden/MLX90621_Arduino_Processing
* Modified by Robert Chapman and Mike Blankenship to work with the GP-001-01 (MLX906XX EVAL Arduino shield) and GP-004-02 (Thermal Imager)
* Original work by:
* 2013 by Felix Bonowski
* Based on a forum post by maxbot: http://forum.arduino.cc/index.php/topic,126244.msg949212.html#msg949212
* This code is in the public domain.
*/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <Wire.h>
#include <OneWire.h>
#include "MLX90621.h"


//TWBR = ((CPU_FREQ / TWI_FREQ_NUNCHUCK) - 16) / 2;

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     9
#define TFT_RST    12 // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     6

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// Instanciate One Wire Library
OneWire  ds(4);  // mikey was on pin 2

// Instance of the Mlexis sensor
MLX90621 sensor; // create an instance of the Sensor class

float tmpTemp;
float DS18B20_temp;
float myFloat[64];
int myLoop=0;
int MyX=0;

void setup(void) {
  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

  //Serial.begin(9600);
  Serial.begin(115200);
  sensor.initialise (2); // Mikey was 2
  tft.fillScreen(ST7735_BLACK);
  
  // optimized lines
  tft.setCursor(0, 90);
  tft.setRotation(3);
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(1);
  //tft.println("Hello World!");
  //tft.setCursor(100,90);
  //tft.print("10");
  //delay(1500);

}

void loop() {
  calculate_DS18B20();
  unsigned long start = millis();
  float total = 0.0;
  int16_t color;
  
  for (int16_t x = 1; x < 16*10; x += 10){
    for (int16_t y = 1; y < 4*20; y += 20){
      color = rand()%65535;
      cellFill(x, y, color); 
    }
  }

  //tft.fillRect(100,90,25,7,0x000000);
  //tft.setCursor(100,90);
  //total = (millis()-start) * 0.001;
  //tft.print(total);
  getVBATvolt();
  getMLXvolt();

    sensor.measure(true); //get new readings from the sensor
  Serial.println("IRSTART");
  Serial.println("MLX90621"); // modified
  for(int y=0;y<4;y++){ //go through all the rows

    for(int x=0;x<16;x++){ //go through all the columns
      //int16_t valueAtXY= sensor.irData[y+x*4]; // extract the temperature at position x/y
      //uint16_t color = getScaleValue(sensor.getTemperature(y+x*4), sensor.getMinTemp(), sensor.getMaxTemp());

      myLoop = ++MyX;
       
      tmpTemp = (sensor.getTemperature(y+x*4));
      Serial.print(tmpTemp);
      //myFloat[myLoop] = tmpTemp;
      //Serial.print(myFloat[myLoop]);
      //Serial.print(" ");
      //delay(100);
      tmpTemp = 0;
      Serial.print(",");
if (myLoop>15)
{
  // do something here
  MyX=0;
  myLoop=0;
  Serial.println();
}
         }
          
  }
 

    // original Serial.print("MLX96021,");
    tmpTemp = (sensor.getAmbient());
  
     //tft.setCursor(0, 10);
    //tft.setTextColor(ST7735_RED);
    //tft.setTextSize(2);
    //tft.println(tmpTemp);
    
    Serial.print("TA=");
    Serial.print(tmpTemp);
    Serial.print(",");
  
    //tft.setCursor(0, 20);
    //tft.setTextColor(ST7735_RED);
    //tft.setTextSize(2);
    //tft.println(tmpTemp);
    
    Serial.print("CPIX=");
    Serial.print(sensor.get_CPIX()); // GET VALUE MIKEY red shOW
    Serial.print(",");
    
    Serial.print("PTAT=");
    Serial.print(sensor.get_PTAT()); // GET VALUE MIKEY
    Serial.print(",");
    
    Serial.print("EMISSIVITY=");
    Serial.print("1"); // GET VALUE MIKEY
    Serial.print(",");
    
    Serial.print("V_TH=");
    //Serial.print(sensor.get_KT1());
    Serial.print("6760"); // GET VALUE MIKEY
    Serial.print(",");
    
    Serial.print("K_T1=");
    Serial.print("23.03"); // GET VALUE MIKEY
    Serial.print(",");
    
    Serial.print("K_T2=");
    Serial.print("0.02"); // GET VALUE MIKEY
    Serial.print(",");
    
    Serial.print("MY_TEMP="); //DS18B20 TEMP
    Serial.print(DS18B20_temp);
    Serial.print(",");
    
    Serial.println("IREND");
    delay(1);

  
total = (millis()-start) ;
 tft.setTextColor(ST7735_BLUE);
  tft.setTextSize(1);
  tft.setCursor(115,120);
  tft.print("FPS: ");
 tft.setTextColor(ST7735_RED);
  tft.setTextSize(1);
  tft.fillRect(140,120,25,7,0x000000);
  tft.setCursor(140,120);
  tft.print(1000/total);
}

//float getScaleValue(float temp, float minTemp, float maxTemp){
//  uint8_t scale = (31.0 / (maxTemp - minTemp))*(temp) + (-1.0) * minTemp * (31.0)/(maxTemp - minTemp);
//  uint16_t color = scale << 11 | scale << 6 | 1 << 5 | scale;
//  return color;
//}



void calculate_DS18B20(){

  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  //byte data[9];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    ///Serial1111.println("No more addresses.");
    ///Serial1111.println();
    ds.reset_search();
    delay(1);
    return;
  }
  
  for( i = 0; i < 8; i++) {
      }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      ///Serial1111.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end
  
  delay(1);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    
      }
  
  // convert the data to actual temperature

  unsigned int raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // count remain gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
    // default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  DS18B20_temp = celsius;

  
  tft.setTextColor(ST7735_BLUE);
  tft.setTextSize(1);
  tft.setCursor(0,120);
  tft.print("INTTEMP: ");
  tft.fillRect(50,120,35,7,0x000000);
  tft.setTextColor(ST7735_RED);
  tft.setCursor(50, 120);
  tft.print(tmpTemp);
  tft.setTextColor(ST7735_YELLOW);
  tft.print((char)248);
  tft.print("C");
  
  tft.setTextColor(ST7735_BLUE);
  tft.setTextSize(1);
  tft.setCursor(0,110);
  tft.print("EXTTEMP: ");
  tft.fillRect(50,110,30,7,0x000000);
  tft.setTextColor(ST7735_RED);
  tft.setCursor(50, 110);
  tft.print(DS18B20_temp);
   tft.setTextColor(ST7735_YELLOW);
  tft.print((char)248);
  tft.print("C");
}

void getMLXvolt() {
  int sensorValue2 = analogRead(A10);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage2 = sensorValue2 * (5.0 / 1023.0);
  tft.setCursor(0,100);
  tft.setTextColor(ST7735_BLUE);
  tft.setTextSize(1);
  tft.print("VMLX: ");
  tft.fillRect(30,100,25,7,0x000000);
  tft.setTextColor(ST7735_RED);
  tft.setCursor(30,100);
  tft.print(voltage2);
  tft.setTextColor(ST7735_YELLOW);
  tft.print(" Volts ");
}


void getVBATvolt() {

int sensorValue = analogRead(A5);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1023.0);
  
  tft.setTextColor(ST7735_BLUE);
  tft.setTextSize(1);
  tft.setCursor(0,90);
  tft.print("VBAT: ");
  tft.fillRect(30,90,25,7,0x000000);
  tft.setTextColor(ST7735_RED);
  tft.setCursor(30,90);
  tft.print(voltage);
  tft.setTextColor(ST7735_YELLOW);
  tft.print( " Volts");
}



void testfastlines(uint16_t color1, uint16_t color2) {
  // width and height have been reversed for a horazontal display.
  int16_t scr_width = tft.height();     // Get screen width
  int16_t scr_height = tft.width();     // Get screen height
  int16_t gtw = 160;
  int16_t gth = 100;
  
  tft.fillScreen(ST7735_BLACK);

  for (int16_t x = 0; x < 5; x++) {
    
    tft.drawFastVLine (x*20, 0,160, 0x414141);
  }

  for (int16_t y = 0; y < 17; y++) {
    if (y == 16 ) {
      tft.drawFastHLine(0,159,4*20, 0x414141);
    } else {
    tft.drawFastHLine (0, y*10, 4*20,0x414141);
    }
  }
}


void cellFill( int16_t x, int16_t y, int16_t color) 
{
 
 tft.fillRect(x,y,9,19,color);
}



