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
  new eventIncoming(e, setMoisture, SET_MOISTURE);
  new eventIncoming(e, setHumidity, SET_HUMIDITY);
  new eventIncoming(e, setSoilTemp, SET_SOIL_TEMP);
  new eventIncoming(e, setAirTemp, SET_AIR_TEMP);
  new eventIncoming(e, setLightOn, SET_LIGHT_ON);
  new eventIncoming(e, setFanOn, SET_FAN_ON);
  new eventIncoming(e, setDistance, SET_DISTANCE);
  new eventIncoming(e, setDistanceAlarm, SET_DISTANCE_ALARM);
  new eventIncoming(e, setWaterLevel, SET_WATER_LEVEL);
  new eventIncoming(e, setHeight, SET_HEIGHT);
  new eventIncoming(e, setGrowMode, SET_GROW_MODE);
  new eventIncoming(e, startFlowering, START_FLOWERING);
  e->check(5);
  Alarm.timerRepeat(5, checkIoT);
  gatherVitals(2);
}

void loop() {
  gatherVitals(60);
}

void gatherVitals(unsigned int delayTime) {
  unsigned int list[10] = {
    GET_MOISTURE, 
    GET_HUMIDITY, 
    GET_AIR_TEMP, 
    GET_SOIL_TEMP, 
    GET_TIME, 
    GET_LIGHT_ON, 
    GET_FAN_ON, 
    GET_DISTANCE,
    GET_WATER_LEVEL, 
    GET_GROW_MODE
  };
  
  DEBUG("----------- Gather Vitals ----------- ");
  for(unsigned int i = 0; i < 10; i++) {
    e->createEvent("", list[i]);
    e->check(delayTime);
  }
}

void checkIoT(void) {
    unsigned long id = 0, value = 0;
    if(Serial3.available()) {
        Serial.println("Incoming!");
        char payload[20];
        unsigned int index = 0;
        char c=0;
        delay(1000);
        do {
            c = Serial3.read();
            payload[index++] = c;
        } while(Serial3.available() && c != '\n');
        payload[index-2]=0;
        sscanf(payload,"%u:%u",&id,&value);
        Serial.println(payload);
        handle(id,value);
    }
}

void handle(const unsigned long id, const unsigned long value) {
  if(id == 1) {
    Serial.println("Particle starting up");
    gatherVitals(2);
  }
}

void setHumidity(const unsigned long v) {
  DEBUG("Humidity is " + String(v));
  Serial3.print(SET_HUMIDITY);
  Serial3.print(":");
  Serial3.println(v);
}

void setMoisture(const unsigned long v) {
  DEBUG("Moisture is " + String(v));
  Serial3.print(SET_MOISTURE);
  Serial3.print(":");
  Serial3.println(v);
}

void setCurrentTime(const unsigned long v) {
  DEBUG("Time is " + String(v));
  Serial3.print(SET_TIME);
  Serial3.print(":");
  Serial3.println(v);
}

void setSoilTemp(const unsigned long v) {
  DEBUG("Soil Temp is " + String(v));
  Serial3.print(SET_SOIL_TEMP);
  Serial3.print(":");
  Serial3.println(v);
}

void setAirTemp(const unsigned long v) {
  DEBUG("Air Temp is " + String(v));
  Serial3.print(SET_AIR_TEMP);
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

void startFlowering(const unsigned long v) {
  DEBUG("Start flowering!");
  Serial3.print(START_FLOWERING);
  Serial3.print(":");
  Serial3.println(v);
}


