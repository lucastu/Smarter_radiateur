#define ENABLE_GxEPD2_GFX 0
#define relayPin 5

#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Wire.h>
#include <DS3231.h>

//Definition of the screen (from the exemple of GxEPD2)
#if defined(__AVR)
#define MAX_DISPLAY_BUFFER_SIZE 800 
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
GxEPD2_BW<GxEPD2_213_T5D, MAX_HEIGHT(GxEPD2_213_T5D)> display(GxEPD2_213_T5D(/*CS=*/ SS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7)); // GDEW0213T5D 104x212, UC8151D
#endif

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

//Variables
bool display_asked = true;
char Hour[] ;
char Minute[] ;
bool h12 ;
bool PM ;
//Objects
DS3231 Clock;

byte lastMinute = 0;
//TOTD = Time Of The Day
byte startHeatingTOTD = 0;
bool heatingProgrammed = false;
bool relayState = false;
byte minuteOftheDay;
byte heatingDuration = 30; //in minute

void setup()
{
  pinMode(relayPin, OUTPUT);
  display.init();
  u8g2Fonts.begin(display); // connect u8g2 procedures to Adafruit GFX

  //For the RTC
  Serial.begin(9600);
  Wire.begin(); //PIN XX & XX
  //setDate(20, 30); 
}

void loop()
{
  if (display_asked){
	  display.fillScreen(GxEPD_WHITE);
	  testdisplayValues();
	  display_asked = false;
	  
	  drawCornerTest(); //test from https://github.com/ZinggJM/GxEPD2/blob/master/examples/GxEPD2_Example/GxEPD2_Example.ino
	  TestPartialUpdate();
  }
	
  //Update time on the screen if needed
  if (Clock.getMinute() != lastMinute) updateTimeDisplay();
  
  //get the time of the day
  TimeOftheDay = ReadTimeOfTheDay();
	
  if (heatingProgrammed){
	  //Add displaying of the info
	  if ((TimeOftheDay >= startHeatingTOTD) && (TimeOftheDay <= (startHeatingTOTD + heatingDuration))){
		  Serial.println("heating");
		  setFilPiloteState(true);
	  }
	  else	{
		 Serial.println("stop heating");
		 setFilPiloteState(false);
		 heatingProgrammed = false;
	 }
  }
  delay(1000);
}

void setDate(byte Hour, byte Minute) {
	//Set Real Time Clock
	Clock.setClockMode(false); 	// set to 24h
	Clock.setMinute(Minute);
	Clock.setHour(Hour);
	Clock.setSecond(0);
	Serial.println("Clock set OK ! ");
}

byte TominuteOfTheDay(byte Hour, byte Minute) { //Transform the time in hour, minute to a count of all the minutes since the day started
     byte minuteOftheDay= Hour*60 + Minute;
     return minuteOftheDay;
}

byte ReadTimeOfTheDay(){
     byte minuteOftheDay= Clock.getHour(h12, PM)*60 + Clock.getMinute();
     return minuteOftheDay;
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

void TestPartialUpdate(){
  display.setRotation(1); // 0--> No rotation ,  1--> rotate 90 deg
  uint16_t bg = GxEPD_WHITE;
  uint16_t fg = GxEPD_BLACK;
  u8g2Fonts.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);            // left to right (this is default)
  u8g2Fonts.setForegroundColor(fg);         // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(bg);         // apply Adafruit GFX color
  u8g2Fonts.setFont(u8g2_font_logisoso32_tr); //u8g2_font_logisoso32_tn--->numbers only to save memory ; u8g2_font_logisoso32_tr , u8g2_font_logisoso32_tf -->numbers&letters
  
  //display.setPartialWindow(0, 0, display.width(), display.height()); //this sets a window for the partial update, so the values can update without refreshing the entire screen.
  display.setPartialWindow(0, 0, display.width()/2, display.height())/2; 
  //https://forum.arduino.cc/t/e-paper-partial-update-i-dont-know-how/527555/8
  //display.setPartialWindow(0, 90, 128, 25); // x,y,width,height
  display.firstPage();
  do
  {
    display.fillScreen(bg);
    // Display first line
    //u8g2Fonts.setCursor(0, 0); 
    display.fillScreen(GxEPD_WHITE);
    display.fillRect(0, 0, display.width(), display.height()) , GxEPD_BLACK);
  }
  while (display.nextPage());	
}

void updateTimeDisplay(){
  display.setRotation(1); // 0--> No rotation ,  1--> rotate 90 deg
  uint16_t bg = GxEPD_WHITE;
  uint16_t fg = GxEPD_BLACK;
  u8g2Fonts.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);            // left to right (this is default)
  u8g2Fonts.setForegroundColor(fg);         // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(bg);         // apply Adafruit GFX color
  u8g2Fonts.setFont(u8g2_font_logisoso32_tr); //u8g2_font_logisoso32_tn--->numbers only to save memory ; u8g2_font_logisoso32_tr , u8g2_font_logisoso32_tf -->numbers&letters
  
  display.setPartialWindow(0, 0, display.width(), display.height()); //this sets a window for the partial update, so the values can update without refreshing the entire screen.
  //https://forum.arduino.cc/t/e-paper-partial-update-i-dont-know-how/527555/8
  //display.setPartialWindow(0, 90, 128, 25); // x,y,width,height
  display.firstPage();
  do
  {
    display.fillScreen(bg);
    // Display first line
    u8g2Fonts.setCursor(0, 0); 
    Hour = Clock.getHour(h12, PM);
    Hour = 2
    Minute = Clock.getMinute();
	  
    u8g2Fonts.printf("%02d", Hour);
    u8g2Fonts.print(":");
    u8g2Fonts.printf(Minute, DEC);
  }
  while (display.nextPage());
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
  u8g2Fonts.setFont(u8g2_font_logisoso32_tr); //u8g2_font_logisoso32_tn--->numbers only to save memory ; u8g2_font_logisoso32_tr , u8g2_font_logisoso32_tf -->numbers&letters
  
  uint16_t x = 50;
  uint16_t y = 50;
	
  display.setPartialWindow(0, 0, display.width(), 296); //this sets a window for the partial update, so the values can update without refreshing the entire screen.
  Serial.print("width:");
  Serial.println(display.width());
  Serial.print("height:");
  Serial.println(display.height());
  //https://forum.arduino.cc/t/e-paper-partial-update-i-dont-know-how/527555/8
  //display.setPartialWindow(0, 90, 128, 25); // x,y,width,height
  display.firstPage();
  do
  {
    display.fillScreen(bg);
    // Display first line
    //u8g2Fonts.setCursor(x, y); 
    //Hour = Clock.getHour(h12, PM);
    //Hour = 2
    //Minute = Clock.getMinute();
	  
    //u8g2Fonts.printf("%02d", Hour);
    //u8g2Fonts.print(":");
    //u8g2Fonts.printf(Minute, DEC);        
    // Display first line
    u8g2Fonts.setCursor(x-50, y+50); 
    u8g2Fonts.print("Mon Chaton");
  }
  while (display.nextPage());
}

void drawCornerTest() //test fonction to be deleted
{
  display.setFullWindow();
  display.setFont(u8g2_font_logisoso32_tr);
  display.setTextColor(GxEPD_BLACK);
  for (uint16_t r = 0; r <= 4; r++)
  {
    display.setRotation(r);
    display.firstPage();
    do
    {
      display.fillScreen(GxEPD_WHITE);
      display.fillRect(0                   , 0                     , 8 , 8 , GxEPD_BLACK);
      display.fillRect(display.width() - 18, 0                     , 16, 16, GxEPD_BLACK);
      display.fillRect(display.width() - 25, display.height() - 25 , 24, 24, GxEPD_BLACK);
      display.fillRect(0                   , display.height() - 33 , 32, 32, GxEPD_BLACK);
      
      display.setCursor(display.width() / 2, display.height() / 2);
      display.print(display.getRotation());
    }
    while (display.nextPage());
    delay(1000);
  }
}
