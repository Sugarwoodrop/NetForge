#include "../../include/netforge/core/port.h"
#include "../../include/netforge/core/nic.h"

int init_port(port_t* port, nic_t* owner){
    if(port == NULL){
        LOG_WARN("cannot initialize NULL port");
        return -1;
    }
    if(owner == NULL){
        LOG_WARN("cannot initialize port with NULL owner");
        return -1;
    }

    port->link = NULL;
    port->nic = owner;
    port->speed = 1;
    
    return 0;
}

void destroy_port(port_t* port){
    if(port == NULL){
        LOG_WARN("tried destroy NULL port");
        return;
    }
    if(port->link == NULL){ //не подключен провод
        port->nic = NULL;
        return;
    }
    destroy_link(port->link);
    port->nic = NULL;
}

void take_on_link(port_t* port, link_t* link){
    if(port == NULL || link == NULL){
        LOG_WARN("tried take on link NULL port or link");
        return;
    }
    if(port->link != NULL){
        LOG_WARN("tried connect link with port is busy");
        return;
    }
    port->link = link;
}

void take_off_link(port_t* port){
    if(port == NULL){
        LOG_WARN("tried take off link NULL port");
        return;
    }
    port->link = NULL;
}

int port_transmit(port_t* port, uint8_t* data, size_t len){
    return link_transmit(port->link, port, data, len);
}

void port_receive(port_t* port, uint8_t* data, size_t len){
    nic_receive(port->nic, data, len);
}