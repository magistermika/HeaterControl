
#include "Heater.h"

const int desiredTemp = 24;
const int relayPin = D1;
const int ledPin = BUILTIN_LED;

Heater::Heater(DHT12& dht) :dht12(dht) {
    pinMode(relayPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);
}

float Heater::getTemp() {
    return _temp;
}

float Heater::getHumi() {
    return _humi;
}

void Heater::heaterMode(const char* mode) {
  if (mode == "manualOn") {
    heaterOn(true);
    return;
  }

  if (mode == "manualOff") {
    heaterOn(false);
    return;
  } 

  if (dht12.cTemp < desiredTemp) {
    heaterOn(true);
  } else {
    heaterOn(false);
  }
}

void Heater::heaterOn(bool on) {
  if (on) {
    digitalWrite(ledPin, LOW);
    digitalWrite(relayPin, HIGH);
    _isHeaterOn = true;
  }
  else {
    digitalWrite(ledPin, HIGH);
    digitalWrite(relayPin, LOW);
    _isHeaterOn = false;
  }
}

bool Heater::heaterState() {
  return _isHeaterOn;
}

void Heater::refresh() {
  if (dht12.get() == 0) {
    _temp = dht12.cTemp;
    _humi = dht12.humidity;
  }  
}