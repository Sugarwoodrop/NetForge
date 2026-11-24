#ifndef NETFORGE_NET_MAC_H
#define NETFORGE_NET_MAC_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "../logger/logger.h"
#include <stdbool.h>
#include <sys/types.h>

typedef struct{
    uint8_t mac[6];
} mac_t;

void mac_random(mac_t *mac);
mac_t mac_get_broadcast();

bool mac_is_equal(mac_t first, mac_t second);
#endif //NETFORGE_NET_MAC_H