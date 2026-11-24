#ifndef NETFORGE_PROTOCOLS_ARP_H
#define NETFORGE_PROTOCOLS_ARP_H

#include "../net/ipv4_addr.h"
#include "../net/mac.h"
#include "../ds/buffer.h"
#include <stdio.h>
#include <sys/types.h>

typedef struct NIC nic_t;

typedef enum { 
    ARP_REQUEST = 1, 
    ARP_REPLY = 2 
} arp_oper_t;

typedef struct {
    ipv4_t ip;
    mac_t  mac;
    time_t expires_at;
} arp_entry_t;

typedef struct{
    arp_entry_t *entries;
    ssize_t size;
    ssize_t capacity;
} arp_table_t;

int arp_table_init(arp_table_t *table);

void arp_destroy(arp_table_t *table);

int get_mac(nic_t *nic, ipv4_t ip, mac_t *out);

int arp_send(ipv4_t dst_ip, nic_t *nic, mac_t dst_mac, arp_oper_t flag);

void arp_receive(buffer_t buffer, nic_t *nic); // вызывает arp_reply() и/или arp_cache_update()

#endif //NETFORGE_PROTOCOLS_ARP_H