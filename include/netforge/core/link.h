#ifndef NETFORGE_CORE_LINK_H
#define NETFORGE_CORE_LINK_H

#include <stddef.h>
#include <stdint.h>

typedef struct Port port_t;

typedef struct Link{
    port_t* left_port;
    port_t* right_port;
} link_t;

int init_link(link_t* link, port_t* left_port, port_t* right_port);
void destroy_link(link_t* link);

int link_transmit(link_t* link, port_t *sender, uint8_t* data, size_t len);

#endif //NETFORGE_CORE_LINK_H