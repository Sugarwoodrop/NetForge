#include "../../include/netforge/devices/host.h"

int init_host(host_t* host){
    if(host == NULL){
        LOG_WARN("cannot initialize NULL host");
        return -1;
    }
    if(init_device(&host->device, 1) == -1){
        return -1;
    }
    
    LOG_INFO("device host initialized");
    return 0;
}

void destroy_host(host_t* host){
    destroy_device(&host->device);
    LOG_INFO("device host destroy");
}

//int ping()