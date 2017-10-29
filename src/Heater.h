#ifndef __HEATER_H_
#define __HEATER_H_

#include <WEMOS_DHT12.h>

class Heater {
public:
    Heater();
    void heaterMode(int mode);
    float getTemp();
    float getHumi();
    void refresh();
    void setDesiredTemp(float setTemp);
    float getDesiredTemp();
    void heaterOn(bool on);
    bool heaterState();
    bool isInAutoMode();
    void setDesiredTempMinus();
    void setDesiredTempPlus();

private:
    DHT12 dht12;
    float _temp;
    float _humi;
    bool _isHeaterOn;
    bool _autoMode;
    float _desiredTemp;
};

#endif