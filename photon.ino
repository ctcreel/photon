#include "DEBUG.h"
#include "../../Projects/HydroManager/Commands.h"
#include "../../Projects/HydroManager/Config.h"
#include "eventStream.h"
#include "eventHandler.h"
#include "eventCallBack.h"
#include "generatorDeviceID.h"
#include "Logger.h"
#include <SD.h>
#include <SPI.h>
#include <RTClib.h>
#include <TextFinder.h>
#include <Time.h>
#include <TimeAlarms.h>
#include <Wire.h>

#define SD_ONE 10
#define SD_TWO 11
#define SD_THREE 12
#define SD_FOUR 13

logger *loggerDevice;
generatorDeviceID gID;
eventStream *e;

RTC_DS1307 RTC;
time_t syncProvider()     //this does the same thing as RTC_DS1307::get()
{
  return RTC.now().unixtime();
}

void setup() {
  Serial.begin(19200);
  Serial2.begin(BAUD_RATE); // XBee
  Serial3.begin(9600); // IoT device
  Wire.begin();
  DEBUG("Starting up...");

  /* Set up clock */ 
  RTC.begin();
  setSyncProvider(syncProvider);     //reference our syncProvider function instead of RTC_DS1307::get()
  DEBUG("Current time is - " + String(hour()) + ":" + String(minute()) + " on " + String(month()) + "-" + String(day()) + "-" + String(year()));

  pinMode(52,OUTPUT);
  digitalWrite(52,HIGH);
  loggerDevice = new logger(SD_ONE, SD_TWO, SD_THREE, SD_FOUR);
  e = new eventStream(&Serial2,&gID);
  new eventIncoming(e, setMoisture, SET_MOISTURE);
  new eventIncoming(e, setHumidity, SET_HUMIDITY);
  new eventIncoming(e, setAirTemp, SET_AIR_TEMP);
  new eventIncoming(e, setLightOnTime, SET_TIME_ON);
  new eventIncoming(e, setLightOn, SET_LIGHT_ON);
  new eventIncoming(e, setFoggerOn, SET_FOGGER_ON);
  new eventIncoming(e, setFanOn, SET_FAN_ON);
  new eventIncoming(e, setDistance, SET_DISTANCE);
  new eventIncoming(e, setDistanceAlarm, SET_DISTANCE_ALARM);
  new eventIncoming(e, setHeight, SET_AVERAGE_HEIGHT);
  new eventIncoming(e, setGrowMode, SET_GROW_MODE);
  new eventIncoming(e, startFlowering, START_FLOWERING);
  e->check(20);
}

void loop() {
  checkIoT();
  e->check(0);
}

void checkIoT(void) {
    unsigned long id = 0, value = 0;
    while(Serial3.available()) {
        Serial.println("Incoming!");
        char payload[200];
        unsigned int index = 0;
        do {
            payload[index++] = Serial3.read();
        } while(Serial3.available() && payload[index-1] != '\n');
        payload[index-1]=0;
        sscanf(payload,"%u:%u",&id,&value);
        Serial.println(payload);
        handle(id,value);
        e->check(0);
    }
}

void handle(const unsigned long id, const unsigned long value) {
  if(id == 1) {
    Serial.println("Particle starting up");
  }
}

void transmit(const unsigned long v, const unsigned int id ) {
  Serial3.print(id);
  Serial3.print(":");
  Serial3.println(v);
}

void setHumidity(const unsigned long v) {
  DEBUG("Humidity is " + String(v));
  loggerDevice->logValue("HUMIDITY.TXT",v);
  transmit(v,SET_HUMIDITY);
}

void setMoisture(const unsigned long v) {
  DEBUG("Moisture is " + String(v));
  loggerDevice->logValue("MOISTURE.TXT",v);
  transmit(v,SET_MOISTURE);
}

void setCurrentTime(const unsigned long v) {
  DEBUG("Time is " + String(v));
  transmit(v,SET_TIME);
}

void setAirTemp(const unsigned long v) {
  DEBUG("Air Temp is " + String(v));
  loggerDevice->logValue("AIRTEMP.TXT",v);
  transmit(v,SET_AIR_TEMP);
}

void setLightOn(const unsigned long v) {
  DEBUG("Light on is " + String(v));
  loggerDevice->logValue("LIGHT.TXT",v);
  transmit(v,SET_LIGHT_ON);
}

void setFanOn(const unsigned long v) {
  DEBUG("Fan on is " + String(v));
  loggerDevice->logValue("FAN.TXT",v);
  transmit(v,SET_FAN_ON);
}

void setDistance(const unsigned long v) {
  DEBUG("Distance is " + String(v));
  loggerDevice->logValue("DISTANCE.TXT",v);
  transmit(v,SET_DISTANCE);
}

void setDistanceAlarm(const unsigned long v) {
  DEBUG("Distance alarm!");
  transmit(v,SET_DISTANCE_ALARM);
}

void setHeight(const unsigned long v) {
  DEBUG("Height is " + String(v));
  loggerDevice->logValue("HEIGHT.TXT",v);
  transmit(v,SET_AVERAGE_HEIGHT);
}

void setLightOnTime(const unsigned long v) {
  DEBUG("Light on time is " + String(v));
  loggerDevice->logValue("LIGHT_ON_TIME.TXT",v);
  transmit(v,SET_TIME_ON);
}

void setFoggerOn(const unsigned long v) {
  DEBUG("Fogger on is " + String(v));
  loggerDevice->logValue("FOGGER.TXT",v);
  transmit(v,SET_FOGGER_ON);
}

void setGrowMode(const unsigned long v) {
  DEBUG("Grow mode is " + String(v));
  loggerDevice->logValue("GROW_MODE.TXT",v);
  transmit(v,SET_GROW_MODE);
}

void startFlowering(const unsigned long v) {
  DEBUG("Start flowering!");
  loggerDevice->logValue("FLOWERING.TXT",v);
  transmit(v,START_FLOWERING);
}

