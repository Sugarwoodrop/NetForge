#ifndef NETFORGE_NET_IPV4_H
#define NETFORGE_NET_IPV4_H

#include "../logger/logger.h"
#include <stdint.h>


typedef struct IPV4{
    uint32_t ip;
} ipv4_t;

int init_ipv4(ipv4_t* ip);

int ipv4_from_string(ipv4_t *ip, const char *str);
void ipv4_to_string(ipv4_t ip, char buf[16]);

#endif //NETFORGE_NET_IPV4_H