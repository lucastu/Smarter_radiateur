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
bool nextday;
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

  if (heatingProgramState ==0){
	  // Bouton Chauffage programmé (horloge)
			  if (digitalRead(button2pin) == LOW ) {
				timeWhenPressed = millis();
				while (digitalRead(button2pin) == LOW){
				  delay(10);
				}
				if (millis()-timeWhenPressed >1000){ //long Press
					Serial.println("Reglage horloge");
					changingClock();
				}else{ //Short Press
						//Ajouteractiver chauffe programmée à 5h (heatingProgramState)
						//Ajouter Changer Boutons écrans
				}
			  }

				if (digitalRead(button1pin) == LOW ) {
					while (digitalRead(button1pin) == LOW) delay(10);
					heatingProgramState  =2;
					stopHeatingTOTD = TimeOftheDay+60;
					//Ajouter Changer Boutons écrans
			  }
		}
		else if (heatingProgramState == 1){  // Mode Programmed heating
			  if (digitalRead(button1pin) == LOW ) {
				while (digitalRead(button1pin) == LOW){
				  delay(10);
				}
				//=> repousser chauffe programmée à + 1h
			  }

			  if (digitalRead(button2pin) == LOW ) {
				while (digitalRead(button2pin) == LOW){
				  delay(10);
				}
				StopHeating();
			  }
			   if (!nextday && TimeOftheDay==0)	 nextday = true;
			   if (nextday){
				   if (TimeOftheDay >= startHeatingTOTD && TimeOftheDay < stopHeatingTOTD) StartHeating();
				   else if (TimeOftheDay > stopHeatingTOTD) StopHeating();
			   }
		}
		else if (heatingProgramState == 2){  // Mode heating Now
			  if (digitalRead(button1pin) == LOW ) {
				while (digitalRead(button1pin) == LOW){
				  delay(10);
				}
				stopHeatingTOTD = stopHeatingTOTD +15; //Add a limit
			  }

			  if (digitalRead(button2pin) == LOW ) {
				while (digitalRead(button2pin) == LOW){
				  delay(10);
				}
				StopHeating();
			  }
			if (TimeOftheDay>=stopHeatingTOTD) StopHeating();
		}
}

void StopHeating(){
	heatingProgramState = 0;
    setFilPiloteState(false);
	// Réaffiche les icones de base
}


void setDate(byte Hour, byte Minute) {
  //Set RTC
  Clock.setClockMode(false);  // set to 24h
  Clock.setMinute(Minute);
  Clock.setHour(Hour);
  Clock.setSecond(0);
  Serial.println("Clock set OK ! ");
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

void changingClock(){
 byte Hour = Clock.getHour(h12, PM);
 byte Minute = Clock.getMinute();
 //Changer les logos en Minute+ et heure +
 //Boucle de reglage où pour sortir il faut
 //Soit du temps sans toucher les boutons (genre 5sec)
 //Soit un appui long sur 1 des deux boutons
 //Remettre les logos normaux
 setDate(Hour, Minute);
}

void displayIcon(byte icon1, byte icon2){
  //Used to set icon above button 1 & 2 according to the variables
  display.setRotation(1);  // 0--> No rotation ,  1--> rotate 90 deg
  uint16_t bg = GxEPD_WHITE;
  uint16_t fg = GxEPD_BLACK;
  u8g2Fonts.setFontMode(1);                    // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);               // left to right (this is default)
  u8g2Fonts.setForegroundColor(fg);            // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(bg);            // apply Adafruit GFX color
  u8g2Fonts.setFont(u8g2_font_logisoso32_tr);  //u8g2_font_logisoso32_tn--->numbers only to save memory ; u8g2_font_logisoso32_tr , u8g2_font_logisoso32_tf -->numbers&letters

  display.setPartialWindow(display.height() / 2, 0, display.width(), display.height() / 2);  // x,y,width,height
  //this sets a window for the partial update, so the values can update without refreshing the entire screen.

  display.firstPage();
  do {
    display.fillScreen(bg);
    u8g2Fonts.setCursor(40, display.height());  // x , y
    u8g2Fonts.print("icon 1");
	
    u8g2Fonts.setCursor(120, display.height());  // x , y	
    u8g2Fonts.print("icon 2");
	
  } while (display.nextPage());	
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
    u8g2Fonts.setCursor(0, 40);  // x , y

    u8g2Fonts.print(Hour);
    u8g2Fonts.print(":");
    u8g2Fonts.print(Minute);
  } while (display.nextPage());
}
