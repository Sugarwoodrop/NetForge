#include "../../include/netforge/net/netstack.h"
#include "../../include/netforge/protocols/ethernet.h"
#include "../../include/netforge/core/nic.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define EXPIRES_ARP 15

typedef struct {
	netstack_t *stack;
	ipv4_t      next_hop;
} arp_resolve_ctx_t;

int stack_init(netstack_t *stack, device_t *owner){
    if(owner == NULL){
        LOG_WARN("cannot initialize NULL");
        return -1;;
    }
    stack->owner = owner;
    if(arp_table_init(&stack->arp_table) == -1){
        return -1;
    }
    if(event_queue_init(&stack->event_queue) == -1){
        arp_destroy(&stack->arp_table);
        return -1;
    }
    stack->pending_count = 0;
    return 0;
}

void stack_destroy(netstack_t *stack){
    arp_destroy(&stack->arp_table);
    event_queue_destroy(&stack->event_queue);
    stack->owner = NULL;
    stack->pending_count = 0;
}

bool arp_pending_has(netstack_t *stack, ipv4_t ip){
    if(stack == NULL){
        return true;
    }
    for(size_t i = 0; i < stack->pending_count; i++){
        if(stack->pending[i].next_hop.ip == ip.ip){
            return true;
        }
    }
    return false;
}

static void arp_resolve_timeout_cb(void *ctx){
	arp_resolve_ctx_t *c     = ctx;
	netstack_t        *stack = c->stack;

	size_t i = 0;
	while (i < stack->pending_count)
	{
		if (stack->pending[i].next_hop.ip != c->next_hop.ip)
		{
			i++;
			continue;
		}

		free(stack->pending[i].packet.bytes);
		stack->pending[i] = stack->pending[--stack->pending_count];
	}

	free(c);
}

int arp_pending_add(netstack_t *stack, nic_t *nic, ipv4_t next_hop, buffer_t packet){
    if(stack == NULL || nic == NULL || stack->pending_count >= ARP_PENDING_MAX){
        LOG_WARN("ERROR input");
        return -1;
    }
    uint8_t *copy = malloc(packet.len);
    if(copy == NULL){
        LOG_ERROR("failed to allocate memory");
        return -1;
    }
    memcpy(copy, packet.bytes, packet.len);

    stack->pending[stack->pending_count++] =(arp_pending_t){
        .nic = nic,
        .next_hop = next_hop,
        .packet =  { .bytes = copy, .len = packet.len },
        .expires_at = EXPIRES_ARP
    };
    arp_resolve_ctx_t *ctx = malloc(sizeof(arp_resolve_ctx_t));
    *ctx = (arp_resolve_ctx_t){.stack = stack, .next_hop = next_hop};
    
    schedule_add_event(&stack->event_queue, (event_t){. callback = arp_resolve_timeout_cb, .ctx = ctx, .execute_at = EXPIRES_ARP});
    return 0;
}

void arp_pending_flush(netstack_t *stack, ipv4_t ip, mac_t mac){
    size_t i = 0;
	while (i < stack->pending_count)
	{
		arp_pending_t *p = &stack->pending[i];
		if (p->next_hop.ip != ip.ip)
		{
			i++;
			continue;
		}

		ethernet_header_t hdr = {
			.dst_mac       = mac,
			.src_mac       = p->nic->mac,
			.ethernet_type = 0x0800,
		};
		ethernet_transmit(p->nic, hdr, p->packet);
		free(p->packet.bytes);

		memmove(&stack->pending[i],
        &stack->pending[i + 1],
        (stack->pending_count - i - 1) * sizeof(stack->pending[0]));
        stack->pending_count--;
	}
};