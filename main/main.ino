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

bool heatingProgramState = 0;  // 0 : not programmed; 1 : for the next day; 2 : now
bool heatingState = false;
unsigned int TimeOftheDay;
unsigned int HeatingTimeLeft;
unsigned int startHeatingTOTD = 0;  //TOTD = Time Of The Day
unsigned int stopHeatingTOTD = 0;

//const unsigned int LastTOTD = 1439;  //Is the last minute of the day (23h59)
byte heatingDuration;
byte lastMinute = 0;
unsigned long timeWhenPressed;
//bool buttonLastState = false;
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
  Wire.begin();  //PIN A2 & A3 ??

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

  if (heatingProgramState == 0) {
    /* 
    No mode active
    Bouton Chauffage programmée
    If long press => Clock Setting Program
    If Short press => set heatingProgramState to 1
    Bouton Chauffe immédiate
    If pressed => set heatingProgramState to 2 
    */

    if (digitalRead(button1pin) == LOW) {
      timeWhenPressed = millis();
      while (digitalRead(button1pin) == LOW) {
        if (millis() - timeWhenPressed > 1000) changingClock();  //long Press Btn1
      }
      if (millis() - timeWhenPressed < 1000) heatingProgramState = 1;    //Short Press Btn 1
    } 
    else if (digitalRead(button2pin) == LOW) {  //Press Btn 2
      heatingProgramState = 2; 
      heatingDuration = 45;
    }
    WaitButtonRelease();
  }

  if (heatingProgramState == 1) {
    /*
    Mode Programmed heating
    when the start time is reached
    then go to immediate heating mode
    */
    if (!isProgrammed) {
      startHeatingTOTD = 5 * 60;
      displayStartHeatingTOTD(startHeatingTOTD);
      displayChoice("+30min", "Annuler");
      isProgrammed=true;
      
      if (startHeatingTOTD > TimeOftheDay) nextday = false;
      else extday = true;
    }
    if (digitalRead(button1pin) == LOW) {  //If want to change programmed start time
      startHeatingTOTD = startHeatingTOTD + 30;
      displayStartHeatingTOTD(startHeatingTOTD);
    }
    if (digitalRead(button2pin) == LOW) {  //If want to stop programmed start
      displayStartHeatingTOTD(0);
      StopHeating();
      isProgrammed=false;
    }

    // Still not sure about this part ...
    //I don't find it elegant
    if (!nextday && TimeOftheDay == 0) nextday = true; // if  new day starts
    if (nextday && TimeOftheDay == startHeatingTOTD) {
      heatingDuration = 60;
      isProgrammed=false;
      heatingProgramState == 2;
    }
    WaitButtonRelease();
  }

  // Mode heating Now
  else if (heatingProgramState == 2) {
    //first start
    if (!heatingState) {
      stopHeatingTOTD = TimeOftheDay + heatingDuration;
      displayStopHeatingTOTD(stopHeatingTOTD - TimeOftheDay);
      StartHeating();
      displayChoice("+15min", "Arrêt");
    }
    
    //If want extra time on the heater
    if (digitalRead(button1pin) == LOW) { 
      if (stopHeatingTOTD - TimeOftheDay<= 165) stopHeatingTOTD = stopHeatingTOTD + 15;  //Set a limit
      HeatingTimeLeft = stopHeatingTOTD - TimeOftheDay;
      displayStopHeatingTOTD(HeatingTimeLeft);
    }
    
    //If want to stop immediat heating   
    if (digitalRead(button2pin) == LOW) {
      StopHeating();  
      displayStopHeatingTOTD(0);
    }
    
    //Refresh time left
    if (HeatingTimeLeft != stopHeatingTOTD - TimeOftheDay) { 
      HeatingTimeLeft = stopHeatingTOTD - TimeOftheDay;
      displayStopHeatingTOTD(HeatingTimeLeft);
    }
    
    //End heating when time is reached
    if (TimeOftheDay >= stopHeatingTOTD) StopHeating();
    WaitButtonRelease();
  }
}

void WaitButtonRelease() {
  /*
  Wait for buttons to be released
  To avoid suprises on long presses
  */
  while (digitalRead(button1pin) == LOW);
  while (digitalRead(button2pin) == LOW);
}

void StartHeating() {
  /*
  Start the heating by setting the relay
  that control the Pilot Wireof the heater
  */
  digitalWrite(relayPin, HIGH);
  //Serial.println("Heating Started");
  heatingState = true;
}

void StopHeating() {
  /*
  Stop the heating by setting the relay
  that control the Pilot Wireof the heater
  */  
  heatingProgramState = 0;
  heatingState = false;
  digitalWrite(relayPin, LOW);
  //Serial.println("Heating Stopped");
  displayChoice("Programme", "Now");
}

void setDate(byte Hour, byte Minute) {
  /*
  Set RTC time from arguments
  and print it to Serial
  */
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
  /*
  return the numbre of minutes passed in the day
  */
  byte minuteOftheDay = Clock.getHour(h12, PM) * 60 + Clock.getMinute();
  return minuteOftheDay;
}

// void setFilPiloteState(bool State) {
//   // Commande le relais du fil pilote
//   if (State) {
//     digitalWrite(relayPin, HIGH);
//     Serial.println("Heating Started");
//   } else {
//     digitalWrite(relayPin, LOW);
//     Serial.println("Heating Stopped");
//   }
// }

void changingClock() {
  /*
  Time changing function
  Button1 to add 1h
  Button2 to add 1min or 10min if long press
  
  leave the function if no action in 4000ms
  */
  unsigned long timeSinceLastPress = millis();
  byte Hour = Clock.getHour(h12, PM);
  byte Minute = Clock.getMinute();
  //Serial.println("Reglage horloge");

  displayChoice("H +", "Min +");
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
      if (Minute < 59) Minute = Minute + 1;  //1 by 1
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
