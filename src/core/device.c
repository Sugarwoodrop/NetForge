#include "../../include/netforge/core/device.h"

int init_device(device_t* device, uint8_t size_nic){ //добавить потом что бы на каждый nic выбералось количество портов через (Device* device, int sizeNIC, ...)
    if(device == NULL){
        return -1;
    }

    device->nic = malloc(sizeof(nic_t) * size_nic);
    if(device->nic == NULL){
        return -1;
    }
    device->size_nic = size_nic; 
    for(int i = 0; i < size_nic; i++){
        if(init_nic(&device->nic[i], 1, device) == -1){
            return -1;
            //НУждно уметь частичную реализацию, то есть если 3 штуки создалось а какойто или несколько не инициализировались
        }
    }
    return 0;
    //netstack пока не трогаю
}

void destroy_device(device_t* device){
    if(device == NULL){
        return;
    }
    for(int i = 0; i < device->size_nic; i++){
        destroy_nic(&device->nic[i]);
    }

    free(device->nic);
    device->size_nic = 0;
    device->nic = NULL;
}

int ping(device_t* tx_device, uint8_t* data, size_t len, mac_t dst_mac){
    ethernet_header_t ethernet_header = {.dst_mac = dst_mac, .src_mac = tx_device->nic[0].mac, .ethernet_type = 0x0800};
    buffer_t buffer = {data, len};
    return ethernet_transmit(&tx_device->nic[0], ethernet_header, buffer);
}

int device_receive(device_t* rx_device){
    return nic_read_buffer(&rx_device->nic[0]);
}