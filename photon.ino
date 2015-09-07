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

void setup() {
  Serial.begin(19200);
  Serial2.begin(BAUD_RATE); // XBee
  Serial3.begin(9600); // IoT device
  DEBUG("Starting up...");
  e = new eventStream(&Serial2,&gID);
  new eventIncoming(e, setMoisture, SET_MOISTURE);
  new eventIncoming(e, setHumidity, SET_HUMIDITY);
  new eventIncoming(e, setSoilTemp, SET_SOIL_TEMP);
  new eventIncoming(e, setAirTemp, SET_AIR_TEMP);
  new eventIncoming(e, setLightOnTime, SET_TIME_ON);
  new eventIncoming(e, setLightOn, SET_LIGHT_ON);
  new eventIncoming(e, setFoggerOn, SET_FOGGER_ON);
  new eventIncoming(e, setFanOn, SET_FAN_ON);
  new eventIncoming(e, setDistance, SET_DISTANCE);
  new eventIncoming(e, setDistanceAlarm, SET_DISTANCE_ALARM);
  new eventIncoming(e, setWaterLevel, SET_WATER_LEVEL);
  new eventIncoming(e, setHeight, SET_HEIGHT);
  new eventIncoming(e, setGrowMode, SET_GROW_MODE);
  new eventIncoming(e, startFlowering, START_FLOWERING);
  e->check(20);
}

void loop() {
  gatherVitals();
  checkIoT();
}

void gatherVitals(void) {
  unsigned int list[] = {
    GET_MOISTURE, 
    GET_HUMIDITY, 
    GET_AIR_TEMP, 
    GET_SOIL_TEMP, 
    GET_TIME,
    GET_LIGHT_ON,
    GET_FAN_ON,
    GET_FOGGER_ON,
    GET_DISTANCE,
    GET_WATER_LEVEL, 
    GET_GROW_MODE,
    GET_TIME_ON
  };
  
  DEBUG("----------- Gather Vitals ----------- ");
  const unsigned int arraySize = (sizeof(list) / sizeof(unsigned int));
  for(unsigned int i = 0; i < arraySize; i++) {
    e->createEvent("", list[i]);
    e->check(300 / arraySize ); // refresh every 5 minutes
  }
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
    gatherVitals();
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
  DEBUG("Water Level is " + String(v));
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

void setLightOnTime(const unsigned long v) {
  DEBUG("Light on time is " + String(v));
  Serial3.print(SET_TIME_ON);
  Serial3.print(":");
  Serial3.println(v);
}

void setFoggerOn(const unsigned long v) {
  DEBUG("Fogger on is " + String(v));
  Serial3.print(SET_FOGGER_ON);
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


