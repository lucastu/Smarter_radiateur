#define ENABLE_GxEPD2_GFX 0
#define relayPin 5
#define button1pin 3
#define button2pin 4

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
::bool display_asked = true;
bool h12;
bool PM;

byte startHeatingTOTD = 0;  //TOTD = Time Of The Day
byte stopHeatingTOTD = 0;
byte heatingProgramState = 0; // 0 : not programmed; 1 : for the next day; 2 : now
bool heatingState = false;
bool relayState = false;
byte TimeOftheDay;
const byte LastTOTD = 1439; //Is the last minute of the day (23h59)
const byte heatingDuration = 60;  //in minute
int HeatingTimeLeft;
byte lastMinute = 0;
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
  pinMode(button1pin, INPUT_PULLUP);
  pinMode(button2pin, INPUT_PULLUP);
  }

void loop() {
  
  TimeOftheDay = ReadTimeOfTheDay();
  //Update time on the screen if needed
  if (TimeOftheDay != lastMinute) {
    updateTimeDisplay();
    Serial.println("Clock display Updated");
  }
  lastMinute = TimeOftheDay;

  if (heatingProgramState) {
    //Add displaying of the info
    if ((TimeOftheDay >= startHeatingTOTD) && (TimeOftheDay <= (startHeatingTOTD + heatingDuration))) {
      Serial.println("heating");
      setFilPiloteState(true);
    } else {
      Serial.println("stop heating");
      setFilPiloteState(false);
      heatingProgramState = 0;
    }
  }
  
  // Bouton Chauffage programmé (horloge)
  if (digitalRead(button2pin) == LOW ) {
    timeWhenPressed = millis();
    while (digitalRead(button2pin) == LOW){
      delay(10);
    }
    if (millis()-timeWhenPressed >1000){
		Serial.println("Reglage horloge");
		changingClock();
    }else{
		Serial.println("Programme du matin");
		if (heatingProgramState == 2) HeatNow(0);
        //HeatProgramming();    
    }
  }
  
  //Bouton chauffage instantannée
  if (digitalRead(button1pin) == LOW ) {
    timeWhenPressed = millis();
    while (digitalRead(button1pin) == LOW){
      delay(10);
    }
    if (millis()-timeWhenPressed >500){
		HeatNow(0);
    }
    else{
        HeatNow(1);    
    }
  }
  HeatingTimeLeft = stopHeatingTOTD - TimeOftheDay;
  if (heatingProgramState = 2 && HeatingTimeLeft =<0) HeatNow(0);
  // si on chauffe mais que le compteur de temps est arrivé a 0 on coupe

  //delay(100);
}

void changingClock(){
 byte Hour = Clock.getHour(h12, PM);
 byte Minute = Clock.getMinute();
 //Boucle de reglage où pour sortir il faut
 //Soit du temps sans toucher les boutons (genre 5sec)
 //Soit un appui long sur 1 des deux boutons
 setDate(Hour, Minute);
}

void HeatNow(byte mode){
  if (mode=0){
    //Stop heating
    heatingProgramState = 0;
    setFilPiloteState(false);
  }
  else if (mode=1){
    if(heatingProgramState == 0){
      // start 60min count down
	  // but can't be after 23h59 (may be possible but lot of work) 
	  if (ReadTimeOfTheDay() + 60 <= LastTOTD){
		  stopHeatingTOTD = ReadTimeOfTheDay() + 60; 
	  }
	  else {
		  stopHeatingTOTD = LastTOTD;
	  }
      heatingProgramState =  2 ;
	  setFilPiloteState(true);
	  
    }
    else {
      // add 15min up to 180min  
      // but can't last after 23h59 (may be possible but lot of work)    
      heatingProgramState =  2 ;
      if (stopHeatingTOTD <= (LastTOTD)){
        stopHeatingTOTD = stopHeatingTOTD + 15 ;     
      }
    }
  }
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
	Serial.println("Heating Started");
  } else {
    digitalWrite(relayPin, LOW);
	Serial.println("Heating Stopped");
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

  display.setPartialWindow(0, 0, display.width(), display.height() / 2);  // x,y,width,height
  //this sets a window for the partial update, so the values can update without refreshing the entire screen.

  display.firstPage();
  byte Hour = Clock.getHour(h12, PM);
  byte Minute = Clock.getMinute();

  do {
    display.fillScreen(bg);
    // Display first line
    u8g2Fonts.setCursor(display.width() / 2 - 0, 40);  // x , y

    //Serial.print(Hour);
    //Serial.print(":");
    //Serial.println(Minute);
    u8g2Fonts.print(Hour);
    u8g2Fonts.print(":");
    u8g2Fonts.print(Minute);
  } while (display.nextPage());
}
