#include "../../include/netforge/net/mac.h"

void mac_random(mac_t *mac)
{
    for (size_t i = 0; i < 6; i++) {
        mac->mac[i] = (uint8_t)(rand() & 0xFF);
    }

    mac->mac[0] &= 0xFE; // Unicast
    mac->mac[0] |= 0x02; // Locally administered
}