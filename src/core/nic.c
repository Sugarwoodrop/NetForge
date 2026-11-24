#include "../../include/netforge/core/nic.h"
#include "../../include/netforge/schedule/schedule.h"
#include "../../include/netforge/core/device.h"
#include <string.h>
#include <stdio.h>

int init_nic(nic_t* nic, uint8_t size_ports, device_t* owner){
    if(nic == NULL){
        LOG_WARN("cannot initialize NULL nic");
        return -1;
    }
    if(owner == NULL){
        LOG_WARN("cannot initialize nic with NULL owner");
        return -1;
    }

    nic->device = owner;
    nic->port = malloc(sizeof(port_t) * size_ports);
    if(nic->port == NULL){
        LOG_ERROR("failed to allocate memory for ports");
        return -1;
    }
    nic->size_ports = size_ports;
    for(int i = 0; i < size_ports; i++){
        if(init_port(&nic->port[i], nic) == -1){
            LOG_ERROR("failed to initialize ports %u", i);

            for (uint8_t j = 0; j < i; j++) {
                destroy_port(&nic->port[j]);
            }
            free(nic->port);
            nic->port = NULL;
            nic->size_ports = 0;

            return -1;
        }
    }
    nic->buffer = malloc(sizeof(*nic->buffer));
    if(nic->buffer == NULL){
        LOG_ERROR("failed to allocate memory for nic buffer");
        destroy_nic(nic);

        return -1;
    }

    if(init_ring_buffer(nic->buffer, 30) == -1){
        destroy_nic(nic);

        return -1;
    }

    mac_random(&nic->mac);
    init_ipv4(&nic->ipv4);

    return 0;
}

void destroy_nic(nic_t* nic){ 
    if(nic == NULL){
        LOG_WARN("tried destroy NULL nic");
        return;
    }
    for(int i = 0; i < nic->size_ports; i++){
        destroy_port(&nic->port[i]);
    }
    free(nic->port);
    nic->device = NULL;
    nic->size_ports = 0;
    nic->port = NULL;
    destroy_ring_buffer(nic->buffer);
    free(nic->buffer);
    nic->buffer = NULL;
}

int nic_transmit(nic_t* nic, uint8_t* data, size_t len){
    return port_transmit(&nic->port[0], data, len);
}

void nic_receive(nic_t* nic, uint8_t* data, size_t len){
    buffer_t item;
    item.bytes = malloc(len);
    if (item.bytes == NULL) {
        LOG_ERROR("failed to allocate memory for received frame");
        return;
    }

    memcpy(item.bytes, data, len);
    item.len = len;

    if(ring_buffer_add_item(nic->buffer, item)){
        free(item.bytes);
    }
    schedule_add_event(&nic->device->netstack.event_queue, (event_t){.callback = device_receive, .ctx = nic->device});
}

buffer_t nic_read_buffer(nic_t* nic){
    return ring_buffer_get(nic->buffer);
}