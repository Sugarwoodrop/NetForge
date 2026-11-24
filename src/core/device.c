#include "../../include/netforge/core/device.h"

int init_device(device_t* device, uint8_t size_nic){ //добавить потом что бы на каждый nic выбералось количество портов через (Device* device, int sizeNIC, ...)
    if(device == NULL){
        LOG_WARN("cannot initialize NULL device");
        return -1;
    }

    device->nic = malloc(sizeof(nic_t) * size_nic);
    if(device->nic == NULL){
        LOG_ERROR("failed to allocate memory for NICs");
        return -1;
    }
    device->size_nic = size_nic; 
    for(int i = 0; i < size_nic; i++){
        if(init_nic(&device->nic[i], 1, device) == -1){
            LOG_ERROR("failed to initialize NIC %u", i);

            for (uint8_t j = 0; j < i; j++) {
                destroy_nic(&device->nic[j]);
            }
            free(device->nic);
            device->nic = NULL;
            device->size_nic = 0;
            return -1;
        }
    }
    if(stack_init(&device->netstack, device) == -1){
        LOG_ERROR("failed to initialize stack_init");
        for (uint8_t j = 0; j < device->size_nic; j++) {
            destroy_nic(&device->nic[j]);
        }
        free(device->nic);
        device->nic = NULL;
        device->size_nic = 0;
        return -1;
    }
    return 0;
}

void destroy_device(device_t* device){
    if(device == NULL){
        LOG_WARN("tried destroy NULL device");
        return;
    }
    for(int i = 0; i < device->size_nic; i++){
        destroy_nic(&device->nic[i]);
    }

    free(device->nic);
    device->size_nic = 0;
    device->nic = NULL;
    stack_destroy(&device->netstack);
}

int ping(device_t* tx_device, uint8_t* data, size_t len, ipv4_t dst_ip){
    arp_pending_t arp_pending;
    mac_t dst_mac;
    buffer_t buffer = {data, len};

    int error = get_mac(tx_device->nic, dst_ip, &dst_mac);
    if(error == -1){
        if(!arp_pending_has(&tx_device->netstack, dst_ip)){
            arp_send(dst_ip, tx_device->nic, (mac_t){0},ARP_REQUEST);
        }
        arp_pending_add(&tx_device->netstack, tx_device->nic, dst_ip, buffer);
        return error;
    }else if (error == -2) {
        return error;
    }
    ethernet_header_t ethernet_header = {.dst_mac = dst_mac, .src_mac = tx_device->nic[0].mac, .ethernet_type = 0x0800};
    return ethernet_transmit(&tx_device->nic[0], ethernet_header, buffer);
}

void device_receive(void *ctx){
    device_t* rx_device = ctx;
    for(int i = 0; i < rx_device->size_nic; i++){
        while(1){
            if(ethernet_receive(nic_read_buffer(&rx_device->nic[i]), rx_device) == -1){
                break; 
            }
        }
    }
}