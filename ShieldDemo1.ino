/* Shield Demo
 *  
 *  Devices
 *  pot A0
 *  lsr A1
 *  LM35  A2
 *      A3 brought out to header
 *  SW1 D2
 *  SW2 D3
 *  DHT11 D4
 *  BUZZ  D5
 *  IRR D6
 *      D7 brought out to header
 *      D8 brought out to header
 *  RGB D9-11
 *    D9  - RED
 *    D10 - BLU
 *    D11 - GRN
 *  RED 12
 *  BLU 13
 *  
 *  SDA 4
 *  ScL 5
 *  
 */

#include <SPI.h>
#include <Wire.h>
#include "D1R32pins.h"

#define IRMP_PROTOCOL_NAMES 1 // Enable protocol number mapping to protocol strings - needs some FLASH. Must before #include <irmp*>
#define IRMP_INPUT_PIN D6

#include <irmpSelectMain15Protocols.h>  // This enables 15 main protocols

/*
 * After setting the modifiers we can include the code and compile it.
 */
#include <irmp.c.h>

IRMP_DATA irmp_data[1];

#define SSD1306_128x64 true   // if display attached.

#if SSD1306_128x64
    #include <Adafruit_GFX.h>
    #include <Adafruit_SSD1306.h>

    #undef OLED_SDA
    #undef OLED_SCK
    
    const byte OLED_SDA = 21;
    const byte OLED_SCK = 22;
    const byte OLED_RST = -1;

    Adafruit_SSD1306 LCD(128, 64, &Wire, OLED_RST);

  #define TOMTHUMB_USE_EXTENDED true // to get cute degree symbol
  #include <Fonts/TomThumb.h>
  const unsigned char DEGREE = 0xb0; // was 8e;  // ignoring comments in TomThumb.h
  #define H 7
  #define LINE1 (H+1)
  #define LINE2 (2*(H+1))   
  #define LINE3 (3*(H+1))
  #define LINE4 (4*(H+1))
  #define LINE5 (5*(H+1))
  #define LINE6 (6*(H+1))
  #define LINE7 (7*(H+1))
  
#endif // SSD1306_128x64

#include <DHTesp.h>
#define DHTPIN D4     // WeMOS D1 R32 multi-shield
// Uncomment whatever type you're using
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
//  DHT dht(DHTPIN, DHTTYPE);
DHTesp dht;   // instantiate object

#define readPot()   analogRead(A0)
#define readLSR()   analogRead(A1)
#define readLM35()  analogRead(A2)
#define LM35toC(v)  (float)100.0 * (((3.30 * v)/1023.0) - 0.5)
#define readSW1()   digitalRead(D2)
#define readSW2()   digitalRead(D3)
#define readSwitch(n) (digitalRead(D2-n) ? "HIGH" : "LOW" )
#define putRED(v)   digitalWrite(D12, v)
#define putBLU(v)   digitalWrite(D13, v)

const int freq = 1000;

const int ledChannelR = 0;
const int ledChannelG = 1;
const int ledChannelB = 2;

const int resolution = 8;

#define PWMRANGE 256

inline void putRGB(int r, int g, int b) { ledcWrite(ledChannelR, r); ledcWrite(ledChannelG, g); ledcWrite(ledChannelB, b); }

void setup() {
  Serial.begin(115200);
  Serial.printf("\n\n\nReset...\n");

  pinMode(D2, INPUT);
  pinMode(D3, INPUT);
  
  pinMode(D5, OUTPUT);
  pinMode(D6, INPUT);

  pinMode(D9, OUTPUT);
  pinMode(D10, OUTPUT);
  pinMode(D11, OUTPUT);

  pinMode(D12, OUTPUT);
  pinMode(D13, OUTPUT);

  irmp_init();
  
// configure LED PWM functionalitites
  ledcSetup(ledChannelR, freq, resolution);
  ledcSetup(ledChannelG, freq, resolution);
  ledcSetup(ledChannelB, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(D9, ledChannelR);
  ledcAttachPin(D10, ledChannelB);
  ledcAttachPin(D11, ledChannelG);

// init DHT11
dht.setup(DHTPIN, DHTesp::DHT11);  // use auto to discover part
delay(1024);  // we know it's DHT-11, so wait a while

//  #if defined(SSD1306_128x64)

    if ( ! Wire.begin(OLED_SDA, OLED_SCK)) {
      Serial.printf("Wire.begin() failed!");
    } else {
      Serial.printf("Wire.begin() success!");
    }
    
    if ( ! LCD.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {
      Serial.printf("LCD.begin() failed!");
    } else {
      Serial.printf("LCD.begin() success!");
    }
    LCD.setFont(&TomThumb);
    LCD.clearDisplay();
    LCD.setTextSize(2);
    LCD.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    LCD.cp437(true);

#define XMIN 0
#define YMIN 0
#define XMAX 127
#define YMAX 63

    LCD.drawFastHLine(XMIN, YMIN, XMAX, SSD1306_WHITE);
    LCD.drawFastHLine(XMIN, YMAX, XMAX, SSD1306_WHITE);
    LCD.drawFastVLine(XMIN, YMIN, YMAX, SSD1306_WHITE);
    LCD.drawFastVLine(XMAX, YMIN, XMAX, SSD1306_WHITE);

    LCD.setCursor(0,10);
    LCD.printf("Test.");
    
    LCD.display();
//  #endif
}

void IRloop() {
  /*
   * Check if new data available and get them
   */
  if (irmp_get_data(&irmp_data[0])) {
    /*
     * Skip repetitions of command
     */
    if (!(irmp_data[0].flags & IRMP_FLAG_REPETITION)) {
      /*
       * Here data is available and is no repetition -> evaluate IR command
       */
    }
    irmp_result_print(&Serial, &irmp_data[0]);
  }  
}
void loop() {
// just a big polling loop, displaying changes and fiddling lights...
static int  newpot, oldpot,
            newlsr, oldlsr,
            newtmp, oldtmp,
            newsw1, oldsw1,
            newsw2, oldsw2,
            redLED, bluLED;

static float newdht, olddht,
             newdhh, olddhh;

static int r,g,b;    

  LCD.clearDisplay();
  
  if ((newpot = readPot()) != oldpot) {
    Serial.printf("Pot changed to %d\n", newpot);
    LCD.setCursor(0,LINE1);
    LCD.printf("Pot: %d\n", newpot);
    oldpot = newpot;
  }

//  Serial.printf("Pot: %d\n",oldpot);
  
  if ((newlsr = readLSR()) != oldlsr) {
    Serial.printf("Light Sensitive Resistor changed to %d\n", newlsr);
    LCD.setCursor(0,LINE2);
    LCD.printf("LSR: %d\n",newlsr);
    oldlsr = newlsr;
  }

//  Serial.printf("LSR: %d\n",oldlsr);
  
  if ((newtmp = readLM35()) != oldtmp) {
    Serial.printf("LM35 reports %d degrees C.\n",LM35toC(newtmp));
    LCD.setCursor(0,LINE3);
    LCD.printf("LM35: %d\n", newtmp);
    oldtmp = newtmp;
  }

// could do A3 pin if we wanted to...

  if ((newsw1 = readSW1()) != oldsw1) {
    Serial.printf("Switch #1 is now %s.\n", readSwitch(0));
    LCD.setCursor(0,LINE4);
    LCD.printf("SW1: %d", newsw1);
    oldsw1 = newsw1;
  }

  if ((newsw2 = readSW2()) != oldsw2) {
    Serial.printf("Switch #2 is now %s.\n", readSwitch(1));
    LCD.setCursor(0,LINE5);
    LCD.printf("SW2: %d", newsw2);
    oldsw2 = newsw2;
  }

  newdht = newdhh = NAN;
    newdht = dht.getTemperature();
    newdhh = dht.getHumidity();

//  Serial.printf("DHT temp:%f, humidity: %f\n",newdht, newdhh);
//  Serial.printf("temp diff: %3.2f, humid diff: %3.2f\n",abs(olddht - newdht), abs(olddhh - newdhh));
  LCD.setCursor(0,LINE6);
  LCD.printf("%3.1f%c %3.1f%c", newdhh, '%', newdht, DEGREE);
  
  if (abs(olddht - newdht) > 0.9) {
    Serial.printf("DHT11 reports temperature is %3.1f degrees C.\n", newdht); 
    olddht = newdht;
  }

  if (abs(olddhh - newdhh) > 0.9) {
    Serial.printf("DHT11 reports humidity is %3.1f percent.\n", newdhh); 
    olddhh = newdhh;
  }

// output/display stuff:

  if (redLED) {
    redLED = 0;
    bluLED = 1;
  } else {
    redLED = 1;
    bluLED = 0;
  }
  
  putRED(redLED);
  putBLU(bluLED);

  r++; g=r+17; b=g+17;
  if (r > PWMRANGE)r = 0;
  if (g > PWMRANGE)g = 0;
  if (b > PWMRANGE)b = 0;
  putRGB(r, g, b);

  LCD.display();

  unsigned long now = millis();
  do {
    IRloop();
  } while ((millis() -now) < 3000);
}
