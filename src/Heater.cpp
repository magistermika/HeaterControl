
#include <WEMOS_DHT12.h>
DHT12 dht12;

const int desiredTemp = 24;
const int relayPin = D1;
int ledPin = BUILTIN_LED;


class Heater {
public:
    Heater();
    void heaterMode(const char* mode);
    float getTemp();
    float getHumi();
    void refresh();
    void setDesiredTemp(float temp);
    void heaterOn(bool on);
    bool heaterState();

private:
    float _temp;
    float _humi;
    bool _isHeaterOn;
};

Heater::Heater() {
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
  else {
    if (dht12.cTemp < desiredTemp) {
      heaterOn(true);
    }
    else {
      heaterOn(false);
    }
    return;
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