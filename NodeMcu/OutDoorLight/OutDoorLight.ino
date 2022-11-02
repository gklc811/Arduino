/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-date-time-ntp-client-server-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <ESP8266WiFi.h>
#include <RTClib.h>
#include <Wire.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


RTC_DS3231 rtc;

int light_on_time = 18;
int light_off_time = 6;
int aerator_on_time = 1;
int aerator_off_time = 5;

int LED = 2;
int LIGHT = 12; //D6
int AERATOR = 14; //D5
int i = 1;
int lf = 0;
char t[32];


const char *ssid     = "GokulRedmi";
const char *password = "123456789";

const long utcOffsetInSeconds = 19800;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup(){
  Wire.begin();
  rtc.begin();
  
  pinMode(LED, OUTPUT);
  pinMode(LIGHT, OUTPUT);
  pinMode(AERATOR, OUTPUT);
  digitalWrite(AERATOR, HIGH);
  digitalWrite(LIGHT, HIGH);
  

  DateTime now = rtc.now();

  // Initialize Aerator
  if (now.hour() >= aerator_on_time && now.hour() <=aerator_off_time) {
    digitalWrite(AERATOR, LOW);
  }
  
  // Initialize Light
  if( (now.hour() >= light_on_time && now.hour() <= 23) || (now.hour() >= 0 && now.hour() <= light_off_time)) {
    digitalWrite(LIGHT, LOW);
    delay (3000);
    digitalWrite(LIGHT, HIGH);
    delay (1000);
    digitalWrite(LIGHT, LOW);
    delay (3000);
    digitalWrite(LIGHT, HIGH);
    delay (1000);
    digitalWrite(LIGHT, LOW);
    lf = 1;
  }


   Serial.begin(115200);
   WiFi.begin(ssid, password);
   WiFiUDP ntpUDP;
   NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
   timeClient.begin();
}

void loop() {
  DateTime now = rtc.now();
  if (i%2==0)
  {
    digitalWrite(LED, HIGH); 
    i = i - 1;
  }
  else
  {
    digitalWrite(LED, LOW); 
    i = i + 1;
  }

  if (now.hour() >= aerator_on_time && now.hour() <=aerator_off_time) {
    digitalWrite(AERATOR, LOW);
  }
  else {
    digitalWrite(AERATOR, HIGH);
  }

  if( (now.hour() >= light_on_time && now.hour() <= 23) || (now.hour() >= 0 && now.hour() <= light_off_time)) {
    if (lf == 0)
    {
      digitalWrite(LIGHT, LOW);
      delay (3000);
      digitalWrite(LIGHT, HIGH);
      delay (1000);
      digitalWrite(LIGHT, LOW);
      delay (3000);
      digitalWrite(LIGHT, HIGH);
      delay (1000);
      digitalWrite(LIGHT, LOW);
      lf = 1;
    }
  }
  else {
    digitalWrite(LIGHT, HIGH);
  }
  sprintf(t, "%02d:%02d:%02d %02d/%02d/%02d",  now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());  
  Serial.print(F("Date/Time from RTC: "));
  Serial.println(t);
  if (WiFi.status() != WL_CONNECTED ) {
     Serial.println("Wifi Not Connected");
  }
  else {
     Serial.println("Wifi Connected");
     timeClient.update();
     unsigned long epochTime = timeClient.getEpochTime();
     struct tm *ptm = gmtime ((time_t *)&epochTime);
     int currentDate = ptm->tm_mday;
     int currentMonth = ptm->tm_mon+1;
     int currentYear = ptm->tm_year+1900;
     sprintf(t, "%02d:%02d:%02d  %02d/%02d/%02d",  timeClient.getHours(), timeClient.getMinutes() ,timeClient.getSeconds(), currentDate, currentMonth, currentYear);
     Serial.print(F("Date/Time from NTP: "));
     Serial.println(t);
     timeClient.update();
     if (currentYear > 2020) {
      Serial.println("Beginnning RTC Write");
      rtc.adjust(DateTime(currentYear, currentMonth, currentDate, timeClient.getHours() , timeClient.getMinutes() , timeClient.getSeconds()));
      for (int j=0;j<20;j++){
       digitalWrite(LED, LOW);
       delay (100);
       digitalWrite(LED, HIGH);
       delay (100);
      }
     }
     else {
      Serial.println("Incorrect NTP time. RTC untouched");
     }
  }
  delay(1000);
  
}
