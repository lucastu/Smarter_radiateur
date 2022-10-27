#define ENABLE_GxEPD2_GFX 0
#define relayPin 5
#define buttonpin 4

#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Wire.h>
#include <DS3231.h>

//Definition of the screen (from the exemple of GxEPD2)
#if defined(__AVR)
#define MAX_DISPLAY_BUFFER_SIZE 800
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
GxEPD2_BW<GxEPD2_213_T5D, MAX_HEIGHT(GxEPD2_213_T5D)> display(GxEPD2_213_T5D(/*CS=*/SS, /*DC=*/8, /*RST=*/9, /*BUSY=*/7));  // GDEW0213T5D 104x212, UC8151D
#endif

//Variables
bool display_asked = true;
//char Hour[] ;
//char Minute[] ;
bool h12;
bool PM;

byte startHeatingTOTD = 0;  //TOTD = Time Of The Day
bool heatingProgrammed = false;
bool relayState = false;
byte TimeOftheDay;
byte heatingDuration = 30;  //in minute
byte lastMinute = 0;
byte Hour;
byte Minute;
unsigned long timeWhenPressed;
int buttonState = 0;
bool buttonLastState = false;
//Objects
DS3231 Clock;
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

void setup() {
  pinMode(relayPin, OUTPUT);
  display.init();
  u8g2Fonts.begin(display);  // connect u8g2 procedures to Adafruit GFX

  //For the RTC
  Serial.begin(9600);
  Wire.begin();  //PIN XX & XX
                 //setDate(17, 41);
  pinMode(buttonpin, INPUT_PULLUP);
}

void loop() {
  if (display_asked) {
    display.fillScreen(GxEPD_WHITE);
    //buttonPressed();
    display_asked = false;
  }
  TimeOftheDay = ReadTimeOfTheDay();
  //Update time on the screen if needed
  if (TimeOftheDay != lastMinute) {
    updateTimeDisplay();
    Serial.println("Reecriture heure");
  }
  lastMinute = TimeOftheDay;
  //get the time of the day

  //Serial.println(TimeOftheDay);
  if (heatingProgrammed) {
    //Add displaying of the info
    if ((TimeOftheDay >= startHeatingTOTD) && (TimeOftheDay <= (startHeatingTOTD + heatingDuration))) {
      Serial.println("heating");
      setFilPiloteState(true);
    } else {
      Serial.println("stop heating");
      setFilPiloteState(false);
      heatingProgrammed = false;
    }
  }
  // if (digitalRead(buttonpin) == LOW && !buttonLastState) {
  //   buttonPressed(true);
  //   buttonLastState = true;
  //   Serial.println("appuyé");
  // } else if (digitalRead(buttonpin) == HIGH && buttonLastState) {
  //   buttonPressed(false);
  //   buttonLastState = false;
  // }
  if (digitalRead(buttonpin) == LOW ) {
    //Serial.println("appuyé");
    timeWhenPressed = millis();
    while (digitalRead(buttonpin) == LOW){
      delay(10);
    }
    if (millis()-timeWhenPressed >500){
      Serial.println("long");
    }
    else{
       Serial.println("court");     
    }
  }
  //delay(100);
}

void setDate(byte Hour, byte Minute) {
  //Set Real Time Clock
  Clock.setClockMode(false);  // set to 24h
  Clock.setMinute(Minute);
  Clock.setHour(Hour);
  Clock.setSecond(0);
  Serial.println("Clock set OK ! ");
}

byte TominuteOfTheDay(byte Hour, byte Minute) {  //Transform the time in hour, minute to a count of all the minutes since the day started
  byte minuteOftheDay = Hour * 60 + Minute;
  return minuteOftheDay;
}

byte ReadTimeOfTheDay() {
  byte minuteOftheDay = Clock.getHour(h12, PM) * 60 + Clock.getMinute();
  return minuteOftheDay;
}


void setFilPiloteState(bool State) {
  // Commande le relais du fil pilote
  if (State) {
    digitalWrite(relayPin, HIGH);
  } else {
    digitalWrite(relayPin, LOW);
  }
}

void updateTimeDisplay() {
  display.setRotation(1);  // 0--> No rotation ,  1--> rotate 90 deg
  uint16_t bg = GxEPD_WHITE;
  uint16_t fg = GxEPD_BLACK;
  u8g2Fonts.setFontMode(1);                    // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);               // left to right (this is default)
  u8g2Fonts.setForegroundColor(fg);            // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(bg);            // apply Adafruit GFX color
  u8g2Fonts.setFont(u8g2_font_logisoso32_tr);  //u8g2_font_logisoso32_tn--->numbers only to save memory ; u8g2_font_logisoso32_tr , u8g2_font_logisoso32_tf -->numbers&letters

  display.setPartialWindow(0, 0, display.width(), display.height() / 2);  //this sets a window for the partial update, so the values can update without refreshing the entire screen.
  //https://forum.arduino.cc/t/e-paper-partial-update-i-dont-know-how/527555/8
  //display.setPartialWindow(0, 90, 128, 25); // x,y,width,height
  display.firstPage();
  do {
    display.fillScreen(bg);
    // Display first line
    u8g2Fonts.setCursor(display.width() / 2 - 0, 40);  // x , y

    Hour = Clock.getHour(h12, PM);
    Hour = 17;
    Minute = Clock.getMinute();
    //Serial.print(Hour);
    //Serial.print(":");
    //Serial.println(Minute);
    u8g2Fonts.print(Hour);
    u8g2Fonts.print(":");
    u8g2Fonts.print(Minute);
  } while (display.nextPage());
}

void buttonPressed(byte mode) {
  //mode1 court
  //mode2 long
  //
  //Serial.println(phrase[0]);
  display.setRotation(1);  // 0--> No rotation ,  1--> rotate 90 deg
  uint16_t bg = GxEPD_WHITE;
  uint16_t fg = GxEPD_BLACK;
  u8g2Fonts.setFontMode(1);                    // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);               // left to right (this is default)
  u8g2Fonts.setForegroundColor(fg);            // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(bg);            // apply Adafruit GFX color
  u8g2Fonts.setFont(u8g2_font_logisoso32_tr);  //u8g2_font_logisoso32_tn--->numbers only to save memory ; u8g2_font_logisoso32_tr , u8g2_font_logisoso32_tf -->numbers&letters

  display.setPartialWindow(0, display.height() / 2, display.width()/2, display.height() / 2);

  //display.setPartialWindow(0, 90, 128, 25); // x,y,width,height
  display.firstPage();
  do {
    display.fillScreen(bg);
    u8g2Fonts.setCursor(0, display.height() - 5);
    u8g2Fonts.print("phrase[0]");
  } while (display.nextPage());
}