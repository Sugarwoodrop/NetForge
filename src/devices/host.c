#include "../../include/netforge/devices/host.h"

int init_host(host_t* host){
    if(host == NULL){
        return -1;
    }
    if(init_device(&host->device, 1) == -1){
        return -1;
    }
    /*if(IinitIPV4(&host->ipv4) == -1){
        return -1;
    }*/
    return 0;
}

void destroy_host(host_t* host){
    destroy_device(&host->device);
}

//int ping()