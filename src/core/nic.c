#include "../../include/netforge/core/nic.h"
#include <string.h>
#include <stdio.h>

int init_nic(nic_t* nic, uint8_t size_ports, device_t* owner){
    if(nic == NULL || owner == NULL){
        return -1;
    }

    nic->device = owner;
    nic->port = malloc(sizeof(port_t) * size_ports);
    if(nic->port == NULL){
        return -1;
    }
    nic->size_ports = size_ports;
    for(int i = 0; i < size_ports; i++){
        if(init_port(&nic->port[i], nic) == -1){
            return -1;
        }
    }
    nic->buffer.size = 0;
    nic->buffer.capacity = 30;
    nic->buffer.buffers = malloc(sizeof(*nic->buffer.buffers) * nic->buffer.capacity);
    if(nic->buffer.buffers == NULL){
        destroy_nic(nic);
        return -1;
    }
    mac_random(&nic->mac);
    return 0;
}

void destroy_nic(nic_t* nic){ // Что делать когда буфер ещё не пустой, но мы делаем дестрой?
    if(nic == NULL){
        return;
    }
    for(int i = 0; i < nic->size_ports; i++){
        destroy_port(&nic->port[i]);
    }
    free(nic->port);
    free(nic->buffer.buffers);
    nic->device = NULL;
    nic->size_ports = 0;
    nic->port = NULL;
}

int nic_transmit(nic_t* nic, uint8_t* data, size_t len){
    return port_transmit(&nic->port[0], data, len);
}
int nic_receive(nic_t* nic, uint8_t* data, size_t len){
    if(nic->buffer.size >= nic->buffer.capacity){
        return -1;
    }
    size_t num = nic->buffer.size == 0 ? 0 : nic->buffer.size+1;
    nic->buffer.buffers[num].data = malloc(sizeof(*nic->buffer.buffers[num].data) * len);
    if(nic->buffer.buffers[num].data == NULL){
        return -1;
    }
    memcpy(nic->buffer.buffers[num].data, data, len);
    nic->buffer.buffers[num].len = len;
    nic->buffer.size++;
    return 0;
}
int nic_read_buffer(nic_t* nic){
    printf("READ -");
    for(size_t i = 0; i < nic->buffer.buffers[nic->buffer.size-1].len; i++){
        printf("%02X ", nic->buffer.buffers[nic->buffer.size-1].data[i]);
    }
    printf("\n");

    ethernet_header_t header; 
    buffer_t* payload = ethernet_parse(nic->buffer.buffers[nic->buffer.size-1], &header);
    
    printf("payload -");
    for(size_t i = 0; i < payload->len; i++){
        printf("%c", payload->data[i]);
    }
    printf("\n");
    free(payload->data);
    free(payload);
    free(nic->buffer.buffers[nic->buffer.size-1].data);
    nic->buffer.buffers[nic->buffer.size-1].data = NULL;
    nic->buffer.buffers[nic->buffer.size-1].len = 0;
    nic->buffer.size--;
    return 0;
}