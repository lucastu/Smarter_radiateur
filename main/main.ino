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
bool h12;
bool PM;

unsigned int startHeatingTOTD = 0;  //TOTD = Time Of The Day
unsigned int stopHeatingTOTD = 0;
unsigned int heatingProgramState = 0;  // 0 : not programmed; 1 : for the next day; 2 : now
bool heatingState = false;
unsigned int TimeOftheDay;
const unsigned int LastTOTD = 1439;  //Is the last minute of the day (23h59)
const byte heatingDuration = 1;      //in minute
unsigned int HeatingTimeLeft;
byte lastMinute = 0;
unsigned long timeWhenPressed;
bool buttonLastState = false;
bool nextday;
bool isProgrammed = false;
uint16_t bg = GxEPD_WHITE;
uint16_t fg = GxEPD_BLACK;
//Objects
DS3231 Clock;
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

const unsigned char logo[294] PROGMEM = { 0X00,0X01,0X2F,0X00,0X30,0X00,
0X00,0X00,0X01,0X00,0X00,0X00,0X00,0X00,0X01,0X80,0X00,0X00,0X00,0X00,0X01,0X80,
0X00,0X00,0X00,0X00,0X01,0X80,0X00,0X00,0X00,0X00,0X01,0X80,0X00,0X00,0X00,0X00,
0X01,0X80,0X00,0X00,0X00,0X00,0X01,0X80,0X00,0X00,0X01,0X80,0X00,0X00,0X01,0X00,
0X01,0XC0,0X00,0X00,0X07,0X00,0X00,0XE0,0X00,0X00,0X06,0X00,0X00,0X70,0X00,0X00,
0X0C,0X00,0X00,0X38,0X03,0XC0,0X18,0X00,0X00,0X10,0X3F,0XF8,0X00,0X00,0X00,0X00,
0X7C,0X7E,0X00,0X00,0X00,0X01,0XE0,0X07,0X00,0X00,0X00,0X03,0X80,0X03,0X80,0X00,
0X00,0X03,0X00,0X01,0XC0,0X00,0X00,0X06,0X00,0X00,0XE0,0X00,0X00,0X0C,0X00,0X00,
0X60,0X00,0X00,0X0C,0X00,0X00,0X70,0X00,0X00,0X18,0X00,0X00,0X30,0X00,0X00,0X18,
0X00,0X00,0X3C,0X00,0X00,0X18,0X00,0X00,0X3E,0X00,0XFE,0X18,0X00,0X00,0X63,0XFE,
0X7E,0X18,0X00,0X00,0X63,0XFE,0X00,0X18,0X00,0X00,0X6B,0X00,0X00,0X18,0X00,0X00,
0X6B,0X00,0X00,0X18,0X00,0X00,0X6B,0X00,0X00,0X0C,0X00,0X00,0X6B,0X00,0X00,0X0C,
0X00,0X00,0X6B,0X00,0X00,0X0E,0X00,0X00,0X6B,0X00,0X00,0X06,0X00,0X00,0XEB,0X00,
0X00,0X03,0X00,0X01,0XEB,0X00,0X00,0X01,0XC0,0X03,0XEB,0X00,0X00,0X00,0XF0,0X0F,
0X6B,0X00,0X00,0X10,0X7F,0XFC,0X6B,0X00,0X00,0X30,0X1F,0XF0,0X6B,0X00,0X00,0X60,
0X00,0X00,0X6B,0X00,0X00,0XC0,0X00,0X00,0X6B,0X00,0X01,0X80,0X00,0X00,0X6B,0X00,
0X01,0X00,0X01,0X00,0XC9,0X80,0X00,0X00,0X01,0X80,0XC9,0X80,0X00,0X00,0X01,0X81,
0X88,0XC0,0X00,0X00,0X01,0X81,0X88,0XC0,0X00,0X00,0X01,0X80,0XC1,0X80,0X00,0X00,
0X01,0X80,0XC1,0X80,0X00,0X00,0X01,0X80,0X7F,0X00,0X00,0X00,0X00,0X00,0X3E,0X00,
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
      if (millis() - timeWhenPressed < 1000) heatingProgramState = 1;    //Short Press Btn 1
    } else if (digitalRead(button2pin) == LOW) heatingProgramState = 2;  //Press Btn 2
    WaitButtonRelease();
  }

  // Mode Programmed heating
  if (heatingProgramState == 1) {
    if (!isProgrammed) {
      startHeatingTOTD = 5;
      displayStartHeatingTOTD(startHeatingTOTD);
      displayChoice("Suivant", "Annuler");
      isProgrammed=true;
    }
    if (digitalRead(button1pin) == LOW) {  //If want to change programmed start
      startHeatingTOTD = startHeatingTOTD + 1;
      displayStartHeatingTOTD(startHeatingTOTD);
    }
    if (digitalRead(button2pin) == LOW) {  //If want to stop programmed start
      //while (digitalRead(button2pin) == LOW) delay(10);
      displayStartHeatingTOTD(0);
      StopHeating();
      isProgrammed=false;
    }

    // Still not sure about this part ...
    //I don't find it elegant
    if (!nextday && TimeOftheDay == 0) nextday = true;
    if (nextday) {
      isProgrammed=false;
      if (TimeOftheDay == startHeatingTOTD) heatingProgramState == 2;
    }
    WaitButtonRelease();
  }

  // Mode heating Now
  else if (heatingProgramState == 2) {
    if (!heatingState) {
      stopHeatingTOTD = TimeOftheDay + heatingDuration;
      displayStopHeatingTOTD(stopHeatingTOTD - TimeOftheDay);
      StartHeating();
      displayChoice("+15min", "Arrêt");
    }
    if (digitalRead(button1pin) == LOW) {
      if (stopHeatingTOTD - TimeOftheDay<= 165) stopHeatingTOTD = stopHeatingTOTD + 15;  
      HeatingTimeLeft = stopHeatingTOTD - TimeOftheDay;
      displayStopHeatingTOTD(HeatingTimeLeft);
    }
    if (digitalRead(button2pin) == LOW) {
      StopHeating();  //If want to stop immediat heating
      displayStopHeatingTOTD(0);
    }
    if (HeatingTimeLeft != stopHeatingTOTD - TimeOftheDay) {
      HeatingTimeLeft = stopHeatingTOTD - TimeOftheDay;
      displayStopHeatingTOTD(HeatingTimeLeft);
    }
    if (TimeOftheDay >= stopHeatingTOTD) StopHeating();
    WaitButtonRelease();
  }
}

void WaitButtonRelease() {
  while (digitalRead(button1pin) == LOW) delay(10);
  while (digitalRead(button2pin) == LOW) delay(10);
}

void StartHeating() {
  setFilPiloteState(true);
  heatingState = true;
  //displayLogo(true);
}

void StopHeating() {
  heatingProgramState = 0;
  heatingState = false;
  setFilPiloteState(false);
  displayChoice("Programme", "Now");
  //displayLogo(false);
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
      if (Minute < 59) Minute = Minute + 1;  //one by one
      else Minute = 0;
      updateTimeDisplay(Hour, Minute);
      while (digitalRead(button2pin) == LOW) {
        if (Minute < 49) Minute = Minute + 10;  //10 by 10 if long press
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