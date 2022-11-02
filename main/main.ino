#define ENABLE_GxEPD2_GFX 0
#define relayPin 5
#define button1pin 4
#define button2pin 3

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
//bool display_asked = true;
bool h12;
bool PM;

byte startHeatingTOTD = 0;  //TOTD = Time Of The Day
byte stopHeatingTOTD = 0;
byte heatingProgramState = 0;  // 0 : not programmed; 1 : for the next day; 2 : now
bool heatingState = false;
bool relayState = false;
byte TimeOftheDay;
const byte LastTOTD = 1439;      //Is the last minute of the day (23h59)
const byte heatingDuration = 1;  //in minute
int HeatingTimeLeft;
byte lastMinute = 0;
unsigned long timeWhenPressed;
int buttonState = 0;
bool buttonLastState = false;
bool nextday;
uint16_t bg = GxEPD_WHITE;
uint16_t fg = GxEPD_BLACK;
//Objects
DS3231 Clock;
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

// const uint8_t BT2_font[62] U8G2_FONT_SECTION("BT_font") =
//   "\2\0\3\2\4\4\2\1\5\11\17\0\0\0\0\0\0\0\0\0\0\0! \32\370\347\313\1\71I\243"
//   "$\312\222R\246\244\226,\211\222(+\205I*\207\0!\5\0\346\0\0\0\0\4\377\377\0";

static unsigned char gImage_a[] = { 0x00,0x01,0x2F,0x00,0x30,0x00,
0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x01,0x80,
0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,
0x01,0x80,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x01,0x80,0x01,0x00,0x03,0x80,
0x01,0xC0,0x00,0x00,0x07,0x00,0x00,0xE0,0x00,0x00,0x0E,0x00,0x00,0x70,0x00,0x00,
0x1C,0x00,0x00,0x38,0x07,0xC0,0x18,0x00,0x00,0x10,0x3F,0xF8,0x10,0x00,0x00,0x00,
0x7E,0x7E,0x00,0x00,0x00,0x01,0xE0,0x0F,0x00,0x00,0x00,0x03,0x80,0x03,0x80,0x00,
0x00,0x07,0x00,0x01,0xC0,0x00,0x00,0x06,0x00,0x00,0xE0,0x00,0x00,0x0E,0x00,0x00,
0x60,0x00,0x00,0x0C,0x00,0x00,0x70,0x00,0x00,0x1C,0x00,0x00,0x30,0x00,0x00,0x18,
0x00,0x00,0x3C,0x00,0x00,0x18,0x00,0x00,0x3E,0x00,0xFF,0x18,0x00,0x00,0x63,0xFE,
0x7E,0x18,0x00,0x00,0x63,0xFE,0x00,0x18,0x00,0x00,0x6B,0x00,0x00,0x18,0x00,0x00,
0x6B,0x00,0x00,0x18,0x00,0x00,0x6B,0x00,0x00,0x1C,0x00,0x00,0x6B,0x00,0x00,0x0C,
0x00,0x00,0x6B,0x00,0x00,0x0E,0x00,0x00,0x6B,0x00,0x00,0x07,0x00,0x00,0xEB,0x00,
0x00,0x03,0x00,0x01,0xEB,0x00,0x00,0x01,0xC0,0x03,0xEB,0x00,0x00,0x00,0xF0,0x0F,
0x6B,0x00,0x00,0x18,0x7F,0xFC,0x6B,0x00,0x00,0x30,0x1F,0xF0,0x6B,0x00,0x00,0x60,
0x00,0x00,0x6B,0x00,0x00,0xC0,0x00,0x00,0x6B,0x00,0x01,0x80,0x00,0x00,0x6B,0x00,
0x01,0x80,0x01,0x80,0xD9,0x80,0x00,0x00,0x01,0x81,0xDD,0xC0,0x00,0x00,0x01,0x81,
0x9C,0xC0,0x00,0x00,0x01,0x81,0x88,0xC0,0x00,0x00,0x01,0x81,0xC1,0xC0,0x00,0x00,
0x01,0x80,0xC1,0x80,0x00,0x00,0x01,0x80,0x7F,0x00,0x00,0x00,0x00,0x00,0x3E,0x00,
};

// prototype needed
void updateTimeDisplay(byte Hour = 100, byte Minute = 0);

void setup() {
  display.init();
  display.firstPage();
  u8g2Fonts.begin(display);  // connect u8g2 procedures to Adafruit GFX
  delay(100);
  display.setRotation(1);  // 0--> No rotation ,  1--> rotate 90 deg

  u8g2Fonts.setFontMode(1);          // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);     // left to right (this is default)
  u8g2Fonts.setForegroundColor(fg);  // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(bg);  // apply Adafruit GFX color
  
  //For the RTC
  Serial.begin(9600);
  Wire.begin();  //PIN XX & XX
  
  // Button and acrtuator init
  pinMode(button1pin, INPUT_PULLUP);
  pinMode(button2pin, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);

  displayChoice("Programme", "Now");
  displayLogo();  
}

void loop() {
  TimeOftheDay = ReadTimeOfTheDay();
  //Update time on the screen if needed
  if (TimeOftheDay != lastMinute) {
    updateTimeDisplay();
    Serial.println("Clock display Updated");
  }
  lastMinute = TimeOftheDay;

  // No mode selected
  if (heatingProgramState == 0) {
    // Bouton Chauffage programmée
    // If long press => Clock Setting Program
    // If Short press => set heatingProgramState to 1 
    // Bouton Chauffe immédiate
    // If pressed => set heatingProgramState to 2
    
    if (digitalRead(button1pin) == LOW) {
      timeWhenPressed = millis();
      while (digitalRead(button1pin) == LOW) {
        if (millis() - timeWhenPressed > 1000) changingClock();  //long Press Btn1
      }
      if (millis() - timeWhenPressed < 1000) heatingProgramState = 1; //Short Press Btn 1
    }
    if (digitalRead(button2pin) == LOW) heatingProgramState = 2; //Press Btn 2
  } 
  
  // Mode Programmed heating  
  if (heatingProgramState == 1) {
    if (!heatingState){
      stopHeatingTOTD = TimeOftheDay + heatingDuration;
      displayStopHeatingTOTD(stopHeatingTOTD - TimeOftheDay);
      StartHeating();
      displayChoice("+15min", "Arrêt");    
    }
    
    if (digitalRead(button1pin) == LOW) { //If want to change programmed start
      //while (digitalRead(button1pin) == LOW) delay(10);
      startHeatingTOTD = startHeatingTOTD + 1;
      displayStartHeatingTOTD(startHeatingTOTD);
    }

    if (digitalRead(button2pin) == LOW) { //If want to stop programmed start
      //while (digitalRead(button2pin) == LOW) delay(10);
      displayStartHeatingTOTD(0);
      StopHeating();
    }
    
    // Still not sure about this part ...
    //I don't find it elegant
    if (!nextday && TimeOftheDay == 0) nextday = true;
    if (nextday) {
      if (TimeOftheDay == startHeatingTOTD) StartHeating();
      else if (TimeOftheDay > stopHeatingTOTD) StopHeating();
    }
  } 
    
  // Mode heating Now
  else if (heatingProgramState == 2) {  
    if (!heatingState){
        startHeatingTOTD = 5;
        displayStartHeatingTOTD(startHeatingTOTD);
        displayChoice("Suivant", "Annuler");
    }
    if (digitalRead(button1pin) == LOW) {
      //while (digitalRead(button1pin) == LOW) delay(10);  
      stopHeatingTOTD = stopHeatingTOTD + 15;  //Add a limit
      HeatingTimeLeft = stopHeatingTOTD - TimeOftheDay;
      displayStopHeatingTOTD(HeatingTimeLeft);
      //while (digitalRead(button1pin) == LOW) delay(10); //If necessary to avoid long press
    }
    if (digitalRead(button2pin) == LOW) StopHeating(); //If want to stop immediat heating
    
    if (HeatingTimeLeft != stopHeatingTOTD - TimeOftheDay) {
      HeatingTimeLeft = stopHeatingTOTD - TimeOftheDay;
      displayStopHeatingTOTD(HeatingTimeLeft);
    }
    if (TimeOftheDay >= stopHeatingTOTD) StopHeating();
  }
}

void StartHeating() {
  setFilPiloteState(true);
  heatingState=true;
  displayLogo(true);
}

void StopHeating() {
  heatingProgramState = 0;
  heatingState=false;
  setFilPiloteState(false);
  displayChoice("Programme", "Now");
  displayLogo(false);
}

void setDate(byte Hour, byte Minute) {
  //Set RTC
  Clock.setClockMode(false);  // set to 24h
  Clock.setMinute(Minute);
  Clock.setHour(Hour);
  Clock.setSecond(0);
  Serial.print("Clock set OK ! to ");
  Serial.print(Hour);
  Serial.print(":");
  Serial.println(Minute);
}

byte ReadTimeOfTheDay() {
  byte minuteOftheDay = Clock.getHour(h12, PM) * 60 + Clock.getMinute();
  return minuteOftheDay;
}

void setFilPiloteState(bool State) {
  // Commande le relais du fil pilote
  if (State) {
    digitalWrite(relayPin, HIGH);
    Serial.println("Heating Started");
  } else {
    digitalWrite(relayPin, LOW);
    Serial.println("Heating Stopped");
  }
}

void changingClock() {
  unsigned long timeSinceLastPress = millis();
  byte Hour = Clock.getHour(h12, PM);
  byte Minute = Clock.getMinute();
  Serial.println("Reglage horloge");
  
  displayChoice("H +", "Min +");
  // Exit the loop if 4000ms passed without any press
  while (millis() - timeSinceLastPress < 4000) {
    //Button Hour +
    while (digitalRead(button1pin) == LOW) {
      timeSinceLastPress = millis();
      if (Hour < 23) Hour = Hour + 1;
      else Hour = 0;
      updateTimeDisplay(Hour, Minute);
    }

    //Button Minute +
    if (digitalRead(button2pin) == LOW) {
      if (Minute < 59) Minute = Minute + 1; //one by one
      else Minute = 0;
      updateTimeDisplay(Hour, Minute);
      while (digitalRead(button2pin) == LOW) {
        if (Minute < 49) Minute = Minute + 10;//10 by 10 if long press
        else Minute = 0;
        updateTimeDisplay(Hour, Minute);
      }
      timeSinceLastPress = millis();
    }
  }
  setDate(Hour, Minute);
  displayChoice("Heure Changee", "");
  delay(1500);
  displayChoice("Programme", "Now");
}
