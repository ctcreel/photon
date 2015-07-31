#include "DEBUG.h"
#include "../../Projects/HydroManager/Commands.h"
#include <SoftwareSerial.h>
#include <TextFinder.h>
#include <Time.h>
#include <TimeAlarms.h>
#include "eventStream.h"
#include "eventHandler.h"
#include "eventCallBack.h"
#include "generatorDeviceID.h"

generatorDeviceID gID;
eventStream *e;
SoftwareSerial ss(11,10);

void setup() {
  Serial.begin(19200);
  Serial3.begin(19200);
  ss.begin(BAUD_RATE);
  DEBUG("Starting up...");
  e = new eventStream(&ss,&gID);
  new eventIncoming(e, setHumidity, SET_HUMIDITY);
  new eventIncoming(e, setTemp, SET_TEMP);
  new eventIncoming(e, setLightOn, SET_LIGHT_ON);
  new eventIncoming(e, setFanOn, SET_FAN_ON);
  new eventIncoming(e, setDistance, SET_DISTANCE);
  new eventIncoming(e, setDistanceAlarm, SET_DISTANCE_ALARM);
  new eventIncoming(e, setWaterLevel, SET_WATER_LEVEL);
  new eventIncoming(e, setHeight, SET_HEIGHT);
  new eventIncoming(e, setGrowMode, SET_GROW_MODE);
  e->check(10);
}

void loop() {
  gatherVitals();
}

void gatherVitals(void) {
  unsigned int list[8] = {GET_HUMIDITY, GET_TEMP, GET_TIME, GET_LIGHT_ON, GET_FAN_ON, GET_DISTANCE,GET_WATER_LEVEL, GET_GROW_MODE};
  DEBUG("----------- Gather Vitals ----------- ");
  for(unsigned int i = 0; i < 8; i++) {
    e->createEvent("", list[i]);
    e->check(225);
  }
}

void setHumidity(const unsigned long v) {
  DEBUG("Humidity is " + String(v));
  Serial3.print(SET_HUMIDITY);
  Serial3.print(":");
  Serial3.println(v);
}

void setCurrentTime(const unsigned long v) {
  DEBUG("Time is " + String(v));
  Serial3.print(SET_TIME);
  Serial3.print(":");
  Serial3.println(v);
}

void setTemp(const unsigned long v) {
  DEBUG("Temp is " + String(v));
  Serial3.print(SET_TEMP);
  Serial3.print(":");
  Serial3.println(v);
}

void setLightOn(const unsigned long v) {
  DEBUG("Light on is " + String(v));
  Serial3.print(SET_LIGHT_ON);
  Serial3.print(":");
  Serial3.println(v);
}

void setFanOn(const unsigned long v) {
  DEBUG("Fan on is " + String(v));
  Serial3.print(SET_FAN_ON);
  Serial3.print(":");
  Serial3.println(v);
}

void setDistance(const unsigned long v) {
  DEBUG("Distance is " + String(v));
  Serial3.print(SET_DISTANCE);
  Serial3.print(":");
  Serial3.println(v);
}

void setDistanceAlarm(const unsigned long v) {
  DEBUG("Distance alarm!");
  Serial3.print(SET_DISTANCE_ALARM);
  Serial3.print(":");
  Serial3.println(v);
}

void setWaterLevel(const unsigned long v) {
  DEBUG("Water Level is" + String(v));
  Serial3.print(SET_WATER_LEVEL);
  Serial3.print(":");
  Serial3.println(v);
}

void setHeight(const unsigned long v) {
  DEBUG("Height is " + String(v));
  Serial3.print(SET_HEIGHT);
  Serial3.print(":");
  Serial3.println(v);
}

void setGrowMode(const unsigned long v) {
  DEBUG("Grow mode is " + String(v));
  Serial3.print(SET_GROW_MODE);
  Serial3.print(":");
  Serial3.println(v);
}


