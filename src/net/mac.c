#include "../../include/netforge/net/mac.h"
#include <stdint.h>
#include <stdio.h>

void mac_random(mac_t *mac)
{
    for (size_t i = 0; i < 6; i++) {
        mac->mac[i] = (uint8_t)(rand() & 0xFF);
    }

    mac->mac[0] &= 0xFE; // Unicast
    mac->mac[0] |= 0x02; // Locally administered
}
mac_t mac_get_broadcast(){
    mac_t mac = {(uint8_t)0xFF, (uint8_t)0xFF,
                (uint8_t)0xFF,(uint8_t)0xFF,
                (uint8_t)0xFF,(uint8_t)0xFF};
    return mac;
}

bool mac_is_equal(mac_t first, mac_t second){
    for(ssize_t i = 0; i < 6; i++){
        if(first.mac[i] != second.mac[i]){
            return false;
        }
    }
    return true;
}