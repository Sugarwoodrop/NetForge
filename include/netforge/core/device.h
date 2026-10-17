#ifndef NETFORGE_CORE_DEVICE_H
#define NETFORGE_CORE_DEVICE_H

#include "nic.h"
#include "../net/netstack.h"
#include "../protocols/ethernet.h"
// Базовый тип устройства, от которого будут наследоваться все остальные типы устройств

typedef struct Device{
    nic_t* nic; // Массив сетевых интерфейсов устройства
    uint8_t size_nic;
    netstack_t netstack;
} device_t;

int init_device(device_t* device, uint8_t size_nic);
void destroy_device(device_t* device);

int ping(device_t* tx_device, uint8_t* data, size_t len, mac_t dst_mac);
int device_receive(device_t* rx_device);

#endif //NETFORGE_CORE_DEVICE_H