#include "../../include/netforge/protocols/ethernet.h"
#include "../../include/netforge/core/nic.h"
#include "../../include/netforge/core/device.h"
#include <stdlib.h>
#include <string.h>

buffer_t*  ethernet_serialize(const ethernet_header_t header, const buffer_t payload){
    /*if(!(header.ethernet_type == 0x0800 || header.ethernet_type == 0x806 || header.ethernet_type == 0x86DD)){
        LOG_WARN(
        "unknown ethernet type \ndst_mac: %02X:%02X:%02X:%02X:%02X:%02X\nsrc_mac: %02X:%02X:%02X:%02X:%02X:%02X",
                                        header.dst_mac.mac[0],
                                        header.dst_mac.mac[1],
                                        header.dst_mac.mac[2],
                                        header.dst_mac.mac[3],
                                        header.dst_mac.mac[4],
                                        header.dst_mac.mac[5],

                                        header.src_mac.mac[0],
                                        header.src_mac.mac[1],
                                        header.src_mac.mac[2],
                                        header.src_mac.mac[3],
                                        header.src_mac.mac[4],
                                        header.src_mac.mac[5]
        );
        return NULL;
    }
    */
    size_t len = (payload.len < 46) ? 46 : payload.len;
    uint8_t* frame = malloc(sizeof(unsigned char)* 6 * 2//src + dst MAC
                    + sizeof(uint16_t) //ethernet_type
                    + sizeof(uint8_t) * len // данные
                    );
    if(frame == NULL){
        LOG_ERROR("failed to allocate memory for frame");
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
    memcpy(frame+14, payload.bytes, payload.len);

    buffer_t* frame_buffer = malloc(sizeof(buffer_t));
    if(frame_buffer == NULL){
        LOG_ERROR("failed to allocate memory for frame_buffer");
        free(frame);
        return NULL;
    }
    frame_buffer->bytes = frame;
    frame_buffer->len = len+14;

    return frame_buffer;
}

buffer_t* ethernet_parse(const buffer_t frame, ethernet_header_t* header){
    if(frame.len < 60 || frame.bytes == NULL){
        LOG_WARN("ethernet_parse: NULL frame.bytes or len <60");
        return NULL;
    }
    if(header == NULL){
        LOG_WARN("ethernet_parse: NULL header");
        return NULL;
    }

    for(size_t i = 0; i < 6; i ++){
        header->dst_mac.mac[i] = frame.bytes[i];
    }
    for(size_t i = 0; i < 6; i++){
        header->src_mac.mac[i] = frame.bytes[6 + i]; 
    }
    header->ethernet_type = (((uint16_t)frame.bytes[12] << 8) & ~0xFF) | frame.bytes[13];
    
    buffer_t* payload = malloc(sizeof(*payload));
    if (payload == NULL){
        LOG_ERROR("failed to allocate memory for payload");
        return NULL;
    }
    payload->len = frame.len - 14;

    payload->bytes = malloc(sizeof(*payload->bytes) * payload->len);
    if(payload->bytes == NULL){
        LOG_ERROR("failed to allocate memory for payload->bytes");
        free(payload);
        return NULL;
    }
    memcpy(payload->bytes, frame.bytes+14, payload->len);
    return payload;
}

int ethernet_transmit(nic_t* nic, const ethernet_header_t header, const buffer_t payload){
    buffer_t* buffer = ethernet_serialize(header, payload);
    if(buffer == NULL){
        LOG_WARN("frame not serialize");
        return -1;
    }

    int error = nic_transmit(nic, buffer->bytes, buffer->len);

    free(buffer->bytes);
    free(buffer);
    return error;
}

int ethernet_receive(buffer_t frame, const device_t *device){ //НАДО ДОБАВИТЬ ОТБРАСЫВАНИЕ КАДРОВ КОТОРЫЕ НЕ ДЛЯ НАС
    if(frame.bytes == NULL){
        return -1;
    }
    ethernet_header_t header;
    buffer_t* buffer = ethernet_parse(frame, &header);
    free(frame.bytes);
    frame.bytes = NULL;
    if (buffer == NULL) {
        return -1;
    }

    //Обработчик всех типов кадров header.ethernet_type == 0x0800 || header.ethernet_type == 0x806 || header.ethernet_type == 0x86DD
    if(header.ethernet_type == 0x806){
        arp_receive(*buffer, device->nic);
    }
    printf("device %p, Payload TXT: ", device);
    for (size_t i = 0; i < buffer->len; i++) {
        unsigned char c = buffer->bytes[i];
        printf("%c", (c >= 32 && c <= 126) ? c : '.');
    }
    printf("\n");


    free(buffer->bytes);
    buffer->bytes = NULL;
    free(buffer);
    buffer = NULL;

    return 0;
}
