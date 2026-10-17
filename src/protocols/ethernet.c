#include "../../include/netforge/protocols/ethernet.h"
#include <stdlib.h>
#include <string.h>

buffer_t*  ethernet_serialize(const ethernet_header_t header, const buffer_t payload){
    if(!(header.ethernet_type == 0x0800 || header.ethernet_type == 0x806 || header.ethernet_type == 0x8DD)){
        return NULL;
    }

    size_t len = (payload.len < 46) ? 46 : payload.len;
    uint8_t* frame = malloc(sizeof(unsigned char)* 6 * 2//src + dst MAC
                    + sizeof(uint16_t) //ethernet_type
                    + sizeof(uint8_t) * len // данные
                    );
    if(frame == NULL){
        return NULL;
    }
    for (size_t i = 0; i < 6; ++i) {
        frame[i] = header.dst_mac.mac[i];
    }
    for (size_t i = 0; i < 6; ++i) {
        frame[6 + i] = header.src_mac.mac[i];
    }
    frame[12] = (uint8_t) (header.ethernet_type >> 8) & 0xFF;
    frame[13] = (uint8_t) (header.ethernet_type & 0xFF);
    for(size_t i = 0; i < 46; i++){
        frame[14 + i] = 0;
    }
    memcpy(frame+14, payload.data, payload.len);

    buffer_t* frame_buffer = malloc(sizeof(buffer_t));
    if(frame_buffer == NULL){
        free(frame);
        return NULL;
    }
    frame_buffer->data = frame;
    frame_buffer->len = len+14;

    return frame_buffer;
}

buffer_t* ethernet_parse(const buffer_t frame, ethernet_header_t* header){
    if(frame.len < 60 || frame.data == NULL){
        return NULL;
    }
    for(size_t i = 0; i < 6; i ++){
        header->dst_mac.mac[i] = frame.data[i];
    }
    for(size_t i = 0; i < 6; i++){
        header->src_mac.mac[i] = frame.data[6 + i]; 
    }
    header->ethernet_type = (((uint16_t)frame.data[12] << 8) & ~0xFF) | frame.data[13];
    
    buffer_t* payload = malloc(sizeof(*payload));
    if (payload == NULL){
        return NULL;
    }
    payload->len = frame.len - 14;

    payload->data = malloc(sizeof(*payload->data) * payload->len);
    if(payload->data == NULL){
        return NULL;
    }
    memcpy(payload->data, frame.data+14, payload->len);
    return payload;
}

int ethernet_transmit(nic_t* nic, const ethernet_header_t header, const buffer_t payload){
    buffer_t* buffer = ethernet_serialize(header, payload);
    if(buffer == NULL){
        return -1;
    }

    int error = nic_transmit(nic, buffer->data, buffer->len);

    free(buffer->data);
    free(buffer);
    return error;
}

/*int ethernet_receive( raw, ethernet_header_t* header, buffer_t* payload){
    //должна просто из циклического буфера по очереди чиать
}
*/