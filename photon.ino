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
#include <LiquidCrystal.h>
#include <Adafruit_RGBLCDShield.h>
#include <lcdManager.h>

#define SD_ONE 10
#define SD_TWO 11
#define SD_THREE 12
#define SD_FOUR 13

logger *loggerDevice;
generatorDeviceID gID;
eventStream *e;
lcdManager *screen;
boolean idle;
unsigned long lastActive;

// Page One
lcdSector *humidity;
lcdSector *moisture;
lcdSector *temp;
lcdSector *foggerStatus;
lcdSector *pumpStatus;
lcdSector *fanStatus;

// Page Two
lcdSector *height;
lcdSector *growMode;
lcdSector *timeNow;
lcdSector *lightStatus;

RTC_DS1307 RTC;
time_t syncProvider()     //this does the same thing as RTC_DS1307::get()
{
  return RTC.now().unixtime();
}

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void setup() {
  pinMode(52,OUTPUT); // Power to devices
  digitalWrite(52,HIGH); // Turn on power
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

  idle = false;
  lastActive = now();

  screen = new lcdManager(16,2,new lcdSectorRGBShieldFactory(16,2,&lcd));
  screen->addPage(0);
  humidity = screen->addSector(0,0,5,0);
  humidity->update("H:??");
  temp = screen->addSector(5,0,6,0);
  temp->update("T:??");
  moisture = screen->addSector(11,0,5,0);
  moisture->update("M:??");
  foggerStatus = screen->addSector(0,1,5,0);
  foggerStatus->update("???");
  fanStatus = screen->addSector(5,1,6,0);
  fanStatus->update("???");
  pumpStatus = screen->addSector(11,1,6,0);
  pumpStatus->update("???");
  
  screen->addPage(1);
  height = screen->addSector(0,0,5,1);
  height->update("H:??");
  lightStatus = screen->addSector(5,0,6,1);
  lightStatus->update("L:???");
  growMode = screen->addSector(11,0,5,1);
  growMode->update("M:???");
  timeNow = screen->addSector(0,1,16,1);
  timeNow->update("Time: HH:MM");
  
  e = new eventStream(&Serial2,&gID);
  new eventIncoming(e, setMoisture, SET_MOISTURE);
  new eventIncoming(e, setHumidity, SET_HUMIDITY);
  new eventIncoming(e, setAirTemp, SET_AIR_TEMP);
  new eventIncoming(e, setLightOnTime, SET_TIME_ON);
  new eventIncoming(e, setLightOn, SET_LIGHT_ON);
  new eventIncoming(e, setFoggerOn, SET_FOGGER_ON);
  new eventIncoming(e, setPump, SET_PUMP_ON);
  new eventIncoming(e, setFanOn, SET_FAN_ON);
  new eventIncoming(e, setDistance, SET_DISTANCE);
  new eventIncoming(e, setDistanceAlarm, SET_DISTANCE_ALARM);
  new eventIncoming(e, setHeight, SET_AVERAGE_HEIGHT);
  new eventIncoming(e, setGrowMode, SET_GROW_MODE);
  new eventIncoming(e, setTimeNow, SET_TIME);
  new eventIncoming(e, startFlowering, START_FLOWERING);
}

void loop() {
  checkIoT();
  checkButtons();
  checkIdle();
  e->check(0);
  checkSettings();
}

void activity(void) {
  lastActive = now();
  screen->turnOn();
  idle = false;
}

void checkSettings(void) {
  static unsigned int command = 0;
  static unsigned long lastChecked = 0;
  void (*const commands[])(void) = {getGrowMode, getTime, getLightOn, getAverageHeight};
  if(now() - lastChecked > 5) {
    (*commands[command])();
    command=(command+1)%(sizeof(commands)/2);
    lastChecked = now();
  }
}

void checkIdle(void) {
  unsigned long elapsed = now() - lastActive;
  
  if(elapsed > 10 && elapsed < 20) {
    screen->activatePage(0);
    idle = true;
  }

  if(now() - lastActive >= 20) {
    screen->turnOff();
  }
}

void checkButtons(void) {
  uint8_t buttons = lcd.readButtons();
  if(buttons) {
    activity();
    if(buttons & BUTTON_RIGHT) {
      screen->activatePage(screen->activePage() + 1 % 2);
    } else if(buttons & BUTTON_LEFT) {
      screen->activatePage(screen->activePage() == 0 ? 1 : screen->activePage() - 1 % 2);
    }
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
  }
}

void transmit(const unsigned long v, const unsigned int id ) {
  Serial3.print(id);
  Serial3.print(":");
  Serial3.println(v);
}

void getGrowMode(void) {
  e->createEvent("0",GET_GROW_MODE);
}

void getTime(void) {
  e->createEvent("0",GET_TIME);
}

void getLightOn(void) {
  e->createEvent("0",GET_LIGHT_ON);
}

void getAverageHeight(void) {
  e->createEvent("0",GET_AVERAGE_HEIGHT);
}

void setTimeNow(const unsigned long v) {
  DEBUG("Time is " + String(v));
  char str[14];
  
  sprintf( str, "Time: %d:%02d",hour(v), minute(v));
  timeNow->update(str);
  transmit(v,SET_TIME);
}

void setHumidity(const unsigned long v) {
  DEBUG("Humidity is " + String(v));
  loggerDevice->logValue("HUMIDITY.TXT",v);
  humidity->update("H:"+String(v));
  transmit(v,SET_HUMIDITY);
}

void setPump(const unsigned long v) {
  DEBUG("Pump is " + String(v));
  loggerDevice->logValue("PUMP.TXT",v);
  v ? pumpStatus->update("On") : pumpStatus->update("Off");
  transmit(v,SET_PUMP_ON);
}

void setMoisture(const unsigned long v) {
  DEBUG("Moisture is " + String(v));
  loggerDevice->logValue("MOISTURE.TXT",v);
  moisture->update("M:"+String(v));
  transmit(v,SET_MOISTURE);
}

void setCurrentTime(const unsigned long v) {
  DEBUG("Time is " + String(v));
  transmit(v,SET_TIME);
  
}

void setAirTemp(const unsigned long v) {
  DEBUG("Air Temp is " + String(v));
  loggerDevice->logValue("AIRTEMP.TXT",v);
  temp->update("T:"+String(v));
  transmit(v,SET_AIR_TEMP);
}

void setLightOn(const unsigned long v) {
  DEBUG("Light on is " + String(v));
  loggerDevice->logValue("LIGHT.TXT",v);
  v ? lightStatus->update("L:On") : lightStatus->update("L:Off");
  transmit(v,SET_LIGHT_ON);
}

void setFanOn(const unsigned long v) {
  DEBUG("Fan on is " + String(v));
  loggerDevice->logValue("FAN.TXT",v);
  v ? fanStatus->update("On") : fanStatus->update("Off");
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
  height->update(String("H:")+String(v));
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
  v ? foggerStatus->update("On") : foggerStatus->update("Off");
  transmit(v,SET_FOGGER_ON);
}

void setGrowMode(const unsigned long v) {
  DEBUG("Grow mode is " + String(v));
  loggerDevice->logValue("GROW_MODE.TXT",v);
  v ? growMode->update("M:Flr") : growMode->update("M:Veg");
  
  transmit(v,SET_GROW_MODE);
}

void startFlowering(const unsigned long v) {
  DEBUG("Start flowering!");
  loggerDevice->logValue("FLOWERING.TXT",v);
  transmit(v,START_FLOWERING);
}

