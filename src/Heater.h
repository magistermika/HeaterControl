#ifndef __HEATER_H_
#define __HEATER_H_

#include <WEMOS_DHT12.h>

class Heater {
public:
    Heater(DHT12& dht12);
    void heaterMode(int mode);
    float getTemp();
    float getHumi();
    void refresh();
    void setDesiredTemp(float temp);
    void heaterOn(bool on);
    bool heaterState();
    bool isInAutoMode();

private:
    DHT12& dht12;
    float _temp;
    float _humi;
    bool _isHeaterOn;
    bool _autoMode;
};

#endif