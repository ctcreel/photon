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
SoftwareSerial photon(50,51);
SoftwareSerial ss(11,10);

void setup() {
  Serial.begin(19200);
  photon.begin(19200);
  ss.begin(BAUD_RATE);
  DEBUG("Starting up...");
  e = new eventStream(&ss,&gID);
  new eventIncoming(e, setHumidity, SET_HUMIDITY);
  new eventIncoming(e, setDesiredHumidity, SET_DESIRED_HUMIDITY);
  new eventIncoming(e, setStartTime, SET_START_TIME);
  new eventIncoming(e, setTimeOn, SET_TIME_ON);
  new eventIncoming(e, setCoolDown, SET_COOL_DOWN);
  new eventIncoming(e, setPumpOn, SET_PUMP_ON);
  new eventIncoming(e, setCurrentTime, SET_TIME);
  new eventIncoming(e, setTemp, SET_TEMP);
  new eventIncoming(e, setLightOn, SET_LIGHT_ON);
  new eventIncoming(e, setFanOn, SET_FAN_ON);
  new eventIncoming(e, lowWaterAlarm, LOW_WATER_ALARM);
  new eventIncoming(e, lowTankAlarm, LOW_TANK_ALARM);
  new eventIncoming(e, setDistance, SET_DISTANCE);
  new eventIncoming(e, setDistanceAlarm, SET_DISTANCE_ALARM);
  e->check(10);
}

void loop() {
  gatherVitals();
  e->check(300);
}

void gatherVitals(void) {
  unsigned int list[6] = {GET_HUMIDITY, GET_TEMP, GET_TIME, GET_LIGHT_ON, GET_FAN_ON, GET_DISTANCE};
  DEBUG("----------- Gather Vitals ----------- ");
  for(unsigned int i = 0; i < 6; i++) {
    e->createEvent("", list[i]);
    e->check(2);
  }
}

void setHumidity(const unsigned long v) {
  DEBUG("Humidity is " + String(v));
  photon.print(SET_HUMIDITY);
  photon.print(":");
  photon.println(v);
}

void setDesiredHumidity(const unsigned long v) {
  
} 

void setStartTime(const unsigned long v) {
  
}

void setTimeOn(const unsigned long v) {
  
}

void setCoolDown(const unsigned long v) {
  
}

void setPumpOn(const unsigned long v) {
  
}

void setCurrentTime(const unsigned long v) {
  DEBUG("Time is " + String(v));
  photon.print(SET_TIME);
  photon.print(":");
  photon.println(v);

}

void setTemp(const unsigned long v) {
  DEBUG("Temp is " + String(v));
  photon.print(SET_TEMP);
  photon.print(":");
  photon.println(v);
}

void setLightOn(const unsigned long v) {
  DEBUG("Light on is " + String(v));
  photon.print(SET_LIGHT_ON);
  photon.print(":");
  photon.println(v);

}

void setFanOn(const unsigned long v) {
  DEBUG("Fan on is " + String(v));
  photon.print(SET_FAN_ON);
  photon.print(":");
  photon.println(v);

}

void lowWaterAlarm(const unsigned long v) {
  
}

void lowTankAlarm(const unsigned long v) {
  
}

void setDistance(const unsigned long v) {
  DEBUG("Distance is " + String(v));
  photon.print(SET_DISTANCE);
  photon.print(":");
  photon.println(v);

}

void setDistanceAlarm(const unsigned long v) {
  DEBUG("Distance alarm!");
//  photon.print(SET_DISTANCE_ALARM);
//  photon.print(":");
//  photon.println(v);
}
