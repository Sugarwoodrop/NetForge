#ifndef NETFORGE_DEVICES_HOST_H
#define NETFORGE_DEVICES_HOST_H

#include "../core/device.h"
#include "../net/ipv4_addr.h"

typedef struct Host{
    device_t device;
    ipv4_t ipv4;
} host_t;

int init_host(host_t* host);
void destroy_host(host_t* host);
//int ping(); // времено просто данные по маку

#endif //NETFORGE_DEVICES_HOST_H