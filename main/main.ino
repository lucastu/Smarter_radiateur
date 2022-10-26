
#define relayPin 5

#define ENABLE_GxEPD2_GFX 0
#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Wire.h>//https://www.arduino.cc/en/reference/wire
#include <DS3231.h>//https://github.com/NorthernWidget/DS3231

#if defined(__AVR)
#define MAX_DISPLAY_BUFFER_SIZE 800 
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
GxEPD2_BW<GxEPD2_213_T5D, MAX_HEIGHT(GxEPD2_213_T5D)> display(GxEPD2_213_T5D(/*CS=*/ SS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7)); // GDEW0213T5D 104x212, UC8151D

#endif

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

bool display_asked = true;
//Variables
int Hour ;
int Minute ;
int Second ;
bool h12 ;
bool PM ;
//Objects
DS3231 Clock;

void setup()
{
  pinMode(relayPin, OUTPUT);
  display.init();
  u8g2Fonts.begin(display); // connect u8g2 procedures to Adafruit GFX
  delay(1000);
  Serial.begin(9600);
 	Wire.begin();
  //setDate(20, 30);
}

void loop()
{
  if (display_asked){
	  display.fillScreen(GxEPD_WHITE);
	  testdisplayValues();
  }
  delay(1000);  
  readRTC();
}

void setDate(int Hour, int Minute) {
 	////Set Real Time Clock

 			Clock.setClockMode(false); 	// set to 24h
 			Clock.setMinute(Minute);
 			Clock.setHour(Hour);
      Clock.setSecond(0);
      Serial.println("Clock set OK ! ");
 	
}
void readRTC( ) { /* function readRTC */
 	////Read Real Time Clock
 	Serial.print(Clock.getHour(h12, PM), DEC); //24-hr
 	Serial.print(":");
 	Serial.print(Clock.getMinute(), DEC);
 	Serial.print(":");
 	Serial.println(Clock.getSecond(), DEC);
}

void setFilPiloteState(bool State){
	// Commande le relais du fil pilote 
	if (State){
		digitalWrite(relayPin, HIGH);
	}
	else{
	digitalWrite(relayPin, LOW);
	}
}

void testdisplayValues()
{
  display.setRotation(1); // 0--> No rotation ,  1--> rotate 90 deg
  uint16_t bg = GxEPD_WHITE;
  uint16_t fg = GxEPD_BLACK;
  u8g2Fonts.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);            // left to right (this is default)
  u8g2Fonts.setForegroundColor(fg);         // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(bg);         // apply Adafruit GFX color
  //u8g2Fonts.setFont(u8g2_font_helvR14_tf);  // select u8g2 font from here:  https://github.com/olikraus/u8g2/wiki/fntlistall

  u8g2Fonts.setFont(u8g2_font_logisoso32_tr); //u8g2_font_logisoso32_tn--->numbers only to save memory ; u8g2_font_logisoso32_tr , u8g2_font_logisoso32_tf -->numbers&letters
  
  uint16_t x = 50;
  uint16_t y = 50;
  display.setPartialWindow(0, 0, display.width(), 296); //this sets a window for the partial update, so the values can update without refreshing the entire screen.
  display.firstPage();
  do
  {
    display.fillScreen(bg);

    // Display first line
    u8g2Fonts.setCursor(x, y); 
    u8g2Fonts.print(Clock.getHour(h12, PM), DEC);
    u8g2Fonts.print(":");
    u8g2Fonts.print(Clock.getMinute(), DEC);        
    // Display first line
    u8g2Fonts.setCursor(x-50, y+50); 
    u8g2Fonts.print("Mon Chaton");



  }
  while (display.nextPage());
}
