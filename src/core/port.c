#include "../../include/netforge/core/port.h"
#include "../../include/netforge/core/nic.h"

int init_port(port_t* port, nic_t* owner){
    if(port == NULL || owner == NULL){
        return -1;
    }
    
    port->link = NULL;
    port->nic = owner;
    port->speed = 1;
    
    return 0;
}
void destroy_port(port_t* port){
    destroy_link(port->link);
    port->nic = NULL;
}

void take_on_link(port_t* port, link_t* link){
    if(port == NULL || link == NULL){
        return;
    }
    //МОЖЕТ ПЕРЕЗАПИСАТЬ СУЩЕСТВУЮЩИЙ КАБЕЛЬ
    port->link = link;
}

void take_off_link(port_t* port){
    if(port == NULL){
        return;
    }
    port->link = NULL;
}

int port_transmit(port_t* port, uint8_t* data, size_t len){
    return link_transmit(port->link, port, data, len);
}

int port_receive(port_t* port, uint8_t* data, size_t len){
    return nic_receive(port->nic, data, len);
}