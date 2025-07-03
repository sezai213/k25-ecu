#include <canbus_manager.h>
#include <mcp2515.h>
#include <global_configuration.h>

#define THROTTLE_MESSAGE_CAN_ID 0x0B4

struct can_frame canMsg;
struct can_frame response;


CanBusManager::CanBusManager() {}

void CanBusManager::initialize() {
    SPI.begin(CAN_SCK, CAN_MISO, CAN_MOSI, CAN_CS_PIN); // SPI pinlerini başlat
    canController = new MCP2515(CAN_CS_PIN);
    canController->reset();
    canController->setBitrate(CAN_500KBPS, MCP_8MHZ);
    canController->setNormalMode();
}

simple_can_package CanBusManager::tick(){

    if (canController->readMessage(&canMsg) == MCP2515::ERROR_OK)
    {

      if (canMsg.can_id == THROTTLE_MESSAGE_CAN_ID && canMsg.can_dlc == 8)
      {
        simple_can_package simplePackage;
        simplePackage.package_type=CANBUS_PACKAGE_TYPE_THROTTLE_VALUE;
        simplePackage.package_value= (canMsg.data[0] << 8) | canMsg.data[1]; // Big-endian
        return simplePackage;
      }
     
    }

    simple_can_package simplePackage;
    simplePackage.package_type=CANBUS_PACKAGE_TYPE_EMPTY;
    return simplePackage;
}




