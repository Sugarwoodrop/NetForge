#ifndef NETFORGE_CORE_NIC_H
#define NETFORGE_CORE_NIC_H

#include "../net/mac.h"
#include "port.h"
#include "../ds/ring_buffer.h"
#include "../protocols/ethernet.h"
#include "../logger/logger.h"
#include "../net/ipv4_addr.h"

typedef struct Device device_t;

typedef struct NIC{
    port_t* port; //массив портов
    device_t* device; //указатель на владельца NIC
    uint8_t size_ports;
    mac_t mac;
    ipv4_t ipv4;
    ring_buffer_t* buffer;
} nic_t;

int init_nic(nic_t* nic, uint8_t size_ports, device_t* owner);
void destroy_nic(nic_t* nic);

int nic_transmit(nic_t* nic, uint8_t* data, size_t len);
void nic_receive(nic_t* nic, uint8_t* data, size_t len);

buffer_t nic_read_buffer(nic_t* nic);

#endif //NETFORGE_CORE_NIC_H