// Based On https://www.aranacorp.com/fr/utilisation-dun-module-ds3231-avec-arduino/
//Libraries
#include <Wire.h>//https://www.arduino.cc/en/reference/wire
#include <DS3231.h>//https://github.com/NorthernWidget/DS3231

//Variables
byte Hour ;
byte Minute ;
byte Second ;
bool h12 ;
bool PM ;
//Objects

DS3231 Clock;
void setup() {
 	//Init Serial USB
 	Serial.begin(9600);
 	Serial.println(F("Initialize System"));
 	Wire.begin();
  
  testsetDate(12, 11, 00);
}

void loop() {
 	readRTC();
  delay(1000);
}

void testsetDate(bool Hour, bool Minute, bool Second){
  // Function just for test purpose
  
  Serial.print("Setting date to : ");
  Serial.print(Hour);
  Serial.print("H ");
  Serial.print(Minute);
  Serial.print("m ");
  Serial.print(Second);
  Serial.println("s");
  
 	setDate(Hour, Minute, Second);
}

void readRTC( ) { /* function readRTC */
 	////Read Real Time Clock
 	Serial.print(Clock.getHour(h12, PM), DEC); //24-hr
 	Serial.print(":");
 	Serial.print(Clock.getMinute(), DEC);
 	Serial.print(":");
 	Serial.println(Clock.getSecond(), DEC);
}

void setDate(bool Hour, bool Minute, bool Second) {
 	////Set Real Time Clock
 	if (Serial.available()) {
 			Clock.setClockMode(false); 	// set to 24h
 			Clock.setSecond(Second);
 			Clock.setMinute(Minute);
 			Clock.setHour(Hour);
      Serial.println("Clock set OK ! ");
 	}
}
