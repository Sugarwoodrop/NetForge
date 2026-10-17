//#include <notcurses/notcurses.h>
#include "../include/netforge/devices/host.h"
#include "../include/netforge/core/link.h"

#include <stdio.h>
#include <time.h>

int main(void){
    srand((unsigned int)time(NULL));

    host_t host1;
    host_t host2;
    init_host(&host1);
    init_host(&host2);

    link_t link;
    init_link(&link, &host1.device.nic[0].port[0], &host2.device.nic[0].port[0]);

    uint8_t str[] = "trolololololololo5242bro";

    ping(&host1.device, str, 24, host2.device.nic[0].mac); //сделать чтобы если у port пустой link то возвращало ошибку, значит надо сделать все эти функции не void а int
    //Убрать в будующем len
    device_receive(&host2.device);

    destroy_host(&host1);
    destroy_host(&host2);
    return 0;
}

//сделать функцию которая возвращяет NIC у которых есть свободные парты и функцию которая возвращяет все свободные порты у данного NIC