#ifndef NETFORGE_CORE_JACK_H
#define NETFORGE_CORE_JACK_H

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "link.h"

typedef struct NIC nic_t;

typedef struct Port{
    link_t* link; // Указатель на провод
    nic_t* nic; // Указатель на владельца порта 
    uint8_t speed; //В гигабитах
} port_t;

int init_port(port_t* port, nic_t* owner);
void destroy_port(port_t* port);

void take_on_link(port_t* port, link_t* link);
void take_off_link(port_t* port);

int port_transmit(port_t* port, uint8_t* data, size_t len);
int port_receive(port_t* port, uint8_t* data, size_t len);

#endif //NETFORGE_CORE_JACK_H