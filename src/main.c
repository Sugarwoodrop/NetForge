//#include <notcurses/notcurses.h>
#include "../include/netforge/devices/host.h"
#include "../include/netforge/core/link.h"
#include "netforge/schedule/schedule.h"

#include <stdbool.h>
#include <time.h>

int main(void){
    srand((unsigned int)time(NULL));

    host_t host1;
    host_t host2;
    init_host(&host1);
    init_host(&host2);

    ipv4_from_string(&host1.device.nic[0].ipv4, "192.168.1.10");
    ipv4_from_string(&host2.device.nic[0].ipv4, "192.168.1.20");

    link_t link;
    init_link(&link, &host1.device.nic[0].port[0], &host2.device.nic[0].port[0]);

    uint8_t str[] = "trolololololololo5242bro";
    uint8_t str2[] = "1234567";
    uint8_t str4[] = "64453aaa";
    uint8_t str3[] = "123aabb";

    ping(&host1.device, str, 24, host2.device.nic[0].ipv4); //сделать чтобы если у port пустой link то возвращало ошибку, значит надо сделать все эти функции не void а int
    ping(&host2.device, str2, 7, host1.device.nic[0].ipv4);
    ping(&host1.device, str4, 8, host2.device.nic[0].ipv4);
    ping(&host2.device, str3, 7, host1.device.nic[0].ipv4);
    ping(&host1.device, str2, 7, host2.device.nic[0].ipv4);
    ping(&host2.device, str4, 8, host1.device.nic[0].ipv4);
    while(true){
        schedule_check_events(&host1.device.netstack.event_queue);
        schedule_check_events(&host2.device.netstack.event_queue);
    }
    destroy_host(&host1);
    destroy_host(&host2);
    return 0;
}

//сделать функцию которая возвращяет NIC у которых есть свободные парты и функцию которая возвращяет все свободные порты у данного NIC