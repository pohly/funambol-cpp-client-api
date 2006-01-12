
#include "spdm/DeviceManager.h"
#include "spdm/DeviceManagerFactory.h"
#include "spdm/PalmDeviceManager.h"


DeviceManagerFactory::DeviceManagerFactory() {};


DeviceManager* DeviceManagerFactory::getDeviceManager() {
    return new PalmDeviceManager();
}