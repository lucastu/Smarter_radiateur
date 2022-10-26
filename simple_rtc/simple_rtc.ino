// Based On https://www.aranacorp.com/fr/utilisation-dun-module-ds3231-avec-arduino/
//Libraries
#include <Wire.h>//https://www.arduino.cc/en/reference/wire
#include <DS3231.h>//https://github.com/NorthernWidget/DS3231

//Variables
int Hour ;
int Minute ;
int Second ;
bool h12 ;
bool PM ;
//Objects
DS3231 Clock;
void setup() {
 	//Init Serial USB
 	Serial.begin(9600);
 	Wire.begin();
  testsetDate(12, 11);
}

void loop() {
 	readRTC();
  delay(1000);
}

void testsetDate(int Hour, int Minute){
  // Function just for test purpose
  
  Serial.print("Setting date to : ");
  Serial.print(Hour);
  Serial.print("H ");
  Serial.print(Minute);
  Serial.print("m ");
  Serial.println("0s");
  
 	setDate(Hour, Minute);
}

void readRTC( ) { /* function readRTC */
 	////Read Real Time Clock
 	Serial.print(Clock.getHour(h12, PM), DEC); //24-hr
 	Serial.print(":");
 	Serial.print(Clock.getMinute(), DEC);
 	Serial.print(":");
 	Serial.println(Clock.getSecond(), DEC);
}

void setDate(int Hour, int Minute) {
 	////Set Real Time Clock

 			Clock.setClockMode(false); 	// set to 24h
 			Clock.setMinute(Minute);
 			Clock.setHour(Hour);
      Clock.setSecond(0);
      Serial.println("Clock set OK ! ");
 	
}
