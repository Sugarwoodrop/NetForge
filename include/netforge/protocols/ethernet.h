#ifndef NETFORGE_PROTOCOLS_ETHERNET_H
#define NETFORGE_PROTOCOLS_ETHERNET_H

#include <stdint.h>
#include "../ds/buffer.h"
#include "../core/nic.h"
#include <stddef.h>

typedef struct{
    mac_t dst_mac;
    mac_t src_mac;
    uint16_t ethernet_type;
} ethernet_header_t;

buffer_t* ethernet_serialize(const ethernet_header_t header, const buffer_t payload);
buffer_t* ethernet_parse(const buffer_t frame, ethernet_header_t* header);

int ethernet_transmit(nic_t* nic, const ethernet_header_t header, const buffer_t payload);
//int ethernet_receive(buffer_t raw, ethernet_header_t* header, buffer_t* payload);

#endif //NETFORGE_PROTOCOLS_ETHERNET_H"

