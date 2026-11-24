#include "../../include/netforge/core/link.h"
#include "../../include/netforge/core/port.h"

int init_link(link_t* link, port_t* left_port, port_t* right_port){
    if(link == NULL || left_port == NULL || right_port == NULL){
        LOG_WARN("cannot initialize link with NULL link or left/right port");
        return -1;
    }
    if (left_port->link != NULL || right_port->link != NULL){
        LOG_WARN("cannot initialize link with port is busy");
        return -1;
    }
    take_on_link(left_port, link);
    take_on_link(right_port, link);

    link->left_port = left_port;
    link->right_port = right_port;

    LOG_INFO("successful connection");
    return 0;
}

void destroy_link(link_t* link){
    if(link == NULL){
        LOG_WARN("tried destroy NULL link");
        return;
    }
    take_off_link(link->left_port);
    take_off_link(link->right_port);

    link->left_port = NULL;
    link->right_port = NULL;
}

int link_transmit(link_t* link, port_t *sender, uint8_t* data, size_t len){
    port_t* receiver;

    if (sender == link->left_port){
        receiver = link->right_port;
    }
    else if (sender == link->right_port){
        receiver = link->left_port;
    }
    else{
        LOG_WARN("receiver not found");
        return -1;
    }

    port_receive(receiver, data, len);
    
    return 0;
}