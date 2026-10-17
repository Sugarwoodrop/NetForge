#ifndef NETFORGE_NET_NETSTACK_H
#define NETFORGE_NET_NETSTACK_H

#include "../core/device.h"

typedef struct {
    device_t *owner;
    //arp таблица
    //таблица комутации
    //таблица маршрутизации
    ///...
} netstack_t;

#endif //NETFORGE_NET_NETSTACK_H