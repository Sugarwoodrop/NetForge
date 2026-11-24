#ifndef NETFORGE_DS_RING_BUFFER_H
#define NETFORGE_DS_RING_BUFFER_H

#include <stddef.h>
#include "../logger/logger.h"
#include "buffer.h"

typedef struct {
    buffer_t *slots;
    size_t head;
    size_t tail;
    size_t capacity;
    size_t size;
} ring_buffer_t;

int init_ring_buffer(ring_buffer_t *buffer, const size_t capacity);
void destroy_ring_buffer(ring_buffer_t *buffer);

int ring_buffer_add_item(ring_buffer_t *buffer, const buffer_t item);
buffer_t ring_buffer_get(ring_buffer_t *buffer);

#endif //NETFORGE_DS_RING_BUFFER_H