//beni delivered the magic

#include "Heater.h"

const float desiredTemp = 26;
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

void Heater::heaterMode(int mode) {
  if (mode == 1) {
    heaterOn(true);
    return;
  }
  if (mode == 0) {
    heaterOn(false);
    return;
  } 
  else { 
    if (dht12.cTemp < desiredTemp) {
        heaterOn(true);
        } 
    else {
        heaterOn(false);
        }
    }
}

void Heater::heaterOn(bool on) {
  if (on == true) {
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