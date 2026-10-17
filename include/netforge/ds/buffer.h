#ifndef NETFORGE_DS_BUFFER_H
#define NETFORGE_DS_BUFFER_H

#include <stddef.h>

typedef struct {
    uint8_t *data;
    size_t len;
} buffer_t;


#endif //NETFORGE_DS_BUFFER_H