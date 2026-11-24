#include "../../include/netforge/protocols/arp.h"
#include "../../include/netforge/protocols/ethernet.h"
#include "../../include/netforge/core/nic.h"
#include "../../include/netforge/schedule/schedule.h"
#include "../../include/netforge/core/device.h"
#include <asm-generic/errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

#define LIFETIME_MATCH 60*10 //60 секунд на количество минут

typedef struct {
	arp_table_t *table;
	ipv4_t  target;
} arp_expire_ctx_t;

int arp_table_init(arp_table_t *table){
    table->capacity = 30;
    table->entries = malloc(sizeof(arp_entry_t) * table->capacity);
    if(table->entries == NULL){
        LOG_ERROR("failed to allocate memory for table->entries");
        return -1;
    }
    table->size = 0;
    return 0;
}

void arp_destroy(arp_table_t *table){
    free(table->entries);
    table->size = 0;
    table->capacity = 0;
}

static void arp_entry_expire_cb(void *ctx){
    if(ctx == NULL){
        return;
    }
    
    arp_expire_ctx_t *arp_ctx = (arp_expire_ctx_t*)ctx;
    for(size_t i = 0; i < arp_ctx->table->size; i++){
        if(arp_ctx->table->entries[i].ip.ip == arp_ctx->target.ip && arp_ctx->table->entries[i].expires_at <= simulator_now()){
            arp_ctx->table->entries[i] = arp_ctx->table->entries[arp_ctx->table->size-1];
            arp_ctx->table->entries[arp_ctx->table->size-1] = (arp_entry_t){0};
            arp_ctx->table->size -= 1;
        }
    }
    free(arp_ctx);
}

static void arp_cache_update(event_queue_t *event_queue, arp_table_t *table, ipv4_t ip, mac_t mac){
    if(table == NULL){
        return;
    }
    for(size_t i = 0; i < table->size; i++){
        if(table->entries[i].ip.ip == ip.ip){
            table->entries[i].mac = mac;
            table->entries[i].expires_at = simulator_now() + LIFETIME_MATCH;
            return;
        }
    }
    if(table->capacity <= table->size){
        size_t new_capacity = table->capacity * 2;
        arp_entry_t *tmp = realloc(table->entries, new_capacity * sizeof(*tmp));
        if(!tmp){
            LOG_ERROR("failed to grow arp table");
            return;
        }
        table->entries = tmp;
        table->capacity *= 2;
    }
    table->entries[table->size].ip = ip;
    table->entries[table->size].mac = mac;
    table->entries[table->size].expires_at = simulator_now() + LIFETIME_MATCH;
    table->size += 1;

    arp_expire_ctx_t *ctx = malloc(sizeof(arp_expire_ctx_t));
    if (ctx == NULL){
        LOG_ERROR("failed to allocate memory for arp_ctx");
	    return;
    }
    *ctx = (arp_expire_ctx_t){ .table = table, .target = ip };
    if(schedule_add_event(event_queue, (event_t){.callback = arp_entry_expire_cb, 
        .ctx = (void*)ctx, .execute_at = table->entries[table->size-1].expires_at}) == -1){
            free(ctx);
        }
    return;
}

int arp_send(ipv4_t dst_ip, nic_t *nic, mac_t dst_mac, arp_oper_t flag){
    if(flag != ARP_REQUEST && flag != ARP_REPLY){
        LOG_WARN("arp_send: error flag");
        return -1;
    }
    if(nic == NULL){
        LOG_WARN("arp_send: NULL nic");
        return -1;
    }
    buffer_t buffer;
    buffer.len = 28;
    buffer.bytes = malloc(sizeof(uint8_t) * buffer.len); //28 байт при HTYPE 2  PTYPE 2 HLEN 1  PLEN 1  OPER 2  SHA 6   SPA 4   THA 6   TPA 4
    if(buffer.bytes == NULL){
        LOG_ERROR("failed to allocate memory for buffer.bytes");
        return -1;
    }
    buffer.bytes[0] = 0x00; buffer.bytes[1]= 0x01; //HTYPE
    buffer.bytes[2] = 0x08; buffer.bytes[3] = 0x00; //PTYPE = 0x0800;
    buffer.bytes[4] = 0x06; //HLEN = 0x06;
    buffer.bytes[5] = 0x04; // PLEN = 0x04;
    
    if(flag == ARP_REQUEST){
        buffer.bytes[6] = 0x00; buffer.bytes[7] = 0x01; //OPER = 0x0001;
    }else {
        buffer.bytes[6] = 0x00; buffer.bytes[7] = 0x02; //OPER = 0x0002;
    }
    memcpy(&buffer.bytes[8], nic->mac.mac, 6); //SHA[6];

    uint32_t spa_net = htonl(nic->ipv4.ip);
    memcpy(&buffer.bytes[14], &spa_net, sizeof(spa_net)); //SPA = src_ip.ip;

    memcpy(&buffer.bytes[18], dst_mac.mac, 6); //THA  dst_mac либо mac либо (mac_t){0} то есть нулевой

    uint32_t tpa_net = htonl(dst_ip.ip);
    memcpy(&buffer.bytes[24], &tpa_net, sizeof(tpa_net));


    ethernet_header_t ethernet_header = {.dst_mac = (flag == ARP_REQUEST) ? mac_get_broadcast() : dst_mac, .src_mac = nic->mac, .ethernet_type = 0x0806};
    int result = ethernet_transmit(nic, ethernet_header, buffer);

    free(buffer.bytes);
    buffer.bytes = NULL;

    return result;
}

void arp_receive(buffer_t buffer, nic_t *nic){
    if (buffer.len < 28 || buffer.bytes == NULL || nic == NULL){
        return;
    }
    uint16_t HTYPE = (uint16_t)(buffer.bytes[0] << 8) | buffer.bytes[1]; 
    uint16_t PTYPE = (uint16_t)(buffer.bytes[2] << 8) | buffer.bytes[3];
    uint8_t HLEN = buffer.bytes[4];
    uint8_t PLEN = buffer.bytes[5];
    uint16_t OPER = (uint16_t)(buffer.bytes[6] << 8) | buffer.bytes[7];

    mac_t SHA;
    memcpy(&SHA.mac, &buffer.bytes[8], 6);

    uint32_t spa_net;
    memcpy(&spa_net,&buffer.bytes[14], sizeof(spa_net));

    uint32_t tpa_net;
    memcpy(&tpa_net,&buffer.bytes[24], sizeof(tpa_net));

    ipv4_t spa = { .ip = ntohl(spa_net) };
    ipv4_t tpa = { .ip = ntohl(tpa_net) };

    if(HTYPE != 0x0001 || PTYPE != 0x0800 || HLEN != 0x06 || PLEN != 0x04 || (OPER != ARP_REQUEST && OPER != ARP_REPLY) || nic->ipv4.ip != tpa.ip){
        return;
    }
    
    arp_cache_update(&nic->device->netstack.event_queue, &nic->device->netstack.arp_table,spa, SHA);
    if(OPER == ARP_REQUEST){
        arp_send(spa, nic, SHA, ARP_REPLY);
    }
    arp_pending_flush(&nic->device->netstack, spa, SHA);
    return;
}

int get_mac(nic_t *nic, ipv4_t ip, mac_t *out){
    if(nic == NULL){
        return -2;
    }
    for(size_t i = 0; i < nic->device->netstack.arp_table.size; i++){
        if(nic->device->netstack.arp_table.entries[i].ip.ip == ip.ip){
            *out = nic->device->netstack.arp_table.entries[i].mac;
            return 0;
        }
    }
    *out = (mac_t){0};
    return -1;
}