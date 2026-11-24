#ifndef NETFORGE_DS_BUFFER_H
#define NETFORGE_DS_BUFFER_H

#include <stddef.h>
#include "../logger/logger.h"
#include <stdint.h>

typedef struct {
    uint8_t *bytes;
    size_t len;
} buffer_t;


#endif //NETFORGE_DS_BUFFER_H