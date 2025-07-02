#include "DHT.h"
#include "thermal_management.h"

ThermalManagement::ThermalManagement(int portIdentifier) : portIdentifier(portIdentifier)
{
}
void ThermalManagement::initialize()
{
    dht = new DHT(portIdentifier, DHT22);
}

float ThermalManagement::readTemperature()
{
    float temperature = dht->readTemperature();
    if (isnan(temperature))
    {
        return -1000;
    }
    return temperature;
}

// Hata kontrolü
