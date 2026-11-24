#ifndef NETFORGE_PROTOCOLS_ETHERNET_H
#define NETFORGE_PROTOCOLS_ETHERNET_H

#include <stdint.h>
#include "../ds/buffer.h"
#include <stddef.h>
#include "../logger/logger.h"
#include "../net/mac.h"

typedef struct NIC nic_t;
typedef struct Device device_t;

typedef struct{
    mac_t dst_mac;
    mac_t src_mac;
    uint16_t ethernet_type;
} ethernet_header_t;

buffer_t* ethernet_serialize(const ethernet_header_t header, const buffer_t payload);
buffer_t* ethernet_parse(const buffer_t frame, ethernet_header_t* header);

int ethernet_transmit(nic_t* nic, const ethernet_header_t header, const buffer_t payload);
int ethernet_receive(const buffer_t frame, const device_t *device);

#endif //NETFORGE_PROTOCOLS_ETHERNET_H"

