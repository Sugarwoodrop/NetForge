#ifndef NETFORGE_NET_NETSTACK_H
#define NETFORGE_NET_NETSTACK_H

#include "../logger/logger.h"
#include "../protocols/arp.h"
#include "../schedule/schedule.h"

#define ARP_PENDING_MAX 16

typedef struct Device device_t;

typedef struct {
	nic_t    *nic;
	ipv4_t    next_hop;
	buffer_t  packet;
	time_t    expires_at;
} arp_pending_t;

typedef struct {
    device_t *owner;
    arp_table_t arp_table;
    event_queue_t event_queue;
    
    arp_pending_t pending[ARP_PENDING_MAX];
    size_t        pending_count;

    //таблица маршрутизации
    ///...
} netstack_t;

int stack_init(netstack_t *stack, device_t *owner);
void stack_destroy(netstack_t *stack);

bool arp_pending_has(netstack_t *stack, ipv4_t ip);
int arp_pending_add(netstack_t *stack, nic_t *nic, ipv4_t next_hop, buffer_t packet);
void arp_pending_flush(netstack_t *stack, ipv4_t ip, mac_t mac);

#endif //NETFORGE_NET_NETSTACK_H