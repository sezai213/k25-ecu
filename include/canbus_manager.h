#include <mcp2515.h>
#ifndef CanBusManager_H
#define CanBusManager_H

#define CANBUS_PACKAGE_TYPE_EMPTY           0x00
#define CANBUS_PACKAGE_TYPE_THROTTLE_VALUE  0x01

struct simple_can_package {
    uint8_t package_type; 
    int64_t package_value; 

};


class CanBusManager {
    public:
    CanBusManager();
    void initialize();
    simple_can_package tick();
    private:
    MCP2515 *canController;
    void processThrottleMessage(can_frame *frame);
      
    };

#endif