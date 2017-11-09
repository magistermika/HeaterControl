//beni delivered the constructor magic

#include "Heater.h"

const int relayPin = D1;
const int ledPin = BUILTIN_LED;

Heater::Heater() {
    DHT12 dht12;
    pinMode(relayPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);
    _desiredTemp = 18;
    _autoMode = true;
}

float Heater::getTemp() {
    return _temp;
}

float Heater::getHumi() {
    return _humi;
}

float Heater::getDesiredTemp() {
    return _desiredTemp;
}

void Heater::setDesiredTemp(float setTemp) {
    _desiredTemp = setTemp;
}

void Heater::setDesiredTempMinus() {
  _desiredTemp--;
}
void Heater::setDesiredTempPlus() {
  _desiredTemp++;
}

void Heater::heaterMode(int mode) {
  if (mode == 1) {
    _autoMode = false;
    heaterOn(true);
    return;
  }
  else if (mode == 0) {
    _autoMode = false;
    heaterOn(false);
    return;
  } 
  else { 
    _autoMode = true;  
    if (dht12.cTemp < _desiredTemp) {
        heaterOn(true);
        } 
    else if (dht12.cTemp >= _desiredTemp) {
            heaterOn(false);
        }
    }
}

bool Heater::isInAutoMode() {
    return _autoMode;
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