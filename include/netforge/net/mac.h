#ifndef NETFORGE_NET_MAC_H
#define NETFORGE_NET_MAC_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct{
    unsigned char mac[6];
} mac_t;

void mac_random(mac_t *mac);

#endif //NETFORGE_NET_MAC_H