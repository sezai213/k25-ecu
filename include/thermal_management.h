#include <DHT.h>
#ifndef ThermalManagement_H
#define ThermalManagement_H

class ThermalManagement
{
    public:
        ThermalManagement(int portIdentifier);
        void initialize();
        float readTemperature();

    private:
        int portIdentifier;
        DHT *dht;
};

#endif