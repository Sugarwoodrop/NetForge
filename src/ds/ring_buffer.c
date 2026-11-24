#include "../../include/netforge/ds/ring_buffer.h"
#include <stdlib.h>

int init_ring_buffer(ring_buffer_t *buffer, const size_t capacity){
    if (buffer == NULL || capacity == 0){
        LOG_WARN("tried init ring buffer with error input");
        return -1;
    }
    buffer->slots = malloc(sizeof(buffer_t) * capacity);
    if(buffer->slots == NULL){
        LOG_ERROR("failed to allocate memory for ring_buffer");
        return -1;
    }
    for(size_t i = 0; i < capacity ;i++){
        buffer->slots[i] = (buffer_t){0};
    }
    buffer->capacity = capacity;
    buffer->head = 0;
    buffer->tail = 0;
    buffer->size = 0;
    return 0;
}

void destroy_ring_buffer(ring_buffer_t *buffer){
    if(buffer == NULL || buffer->slots == NULL){
        LOG_WARN("tried destroy NULL ring buffer");
        return;
    }
    for(size_t i = 0; i < buffer->capacity; i++){
        free(buffer->slots[i].bytes);
        buffer->slots[i].bytes = NULL;
        buffer->slots[i] = (buffer_t){0};
    }

    free(buffer->slots);
    buffer->slots = NULL;
    buffer->capacity = 0;
    buffer->head = 0;
    buffer->tail = 0;
    buffer->size = 0;
}

int ring_buffer_add_item(ring_buffer_t *buffer, const buffer_t item){
    if(buffer == NULL || buffer->slots == NULL){
        LOG_WARN("cannot add NULL ring buffer");
        return -1;
    }
    if(buffer->capacity == buffer->size){
        return -1;
    }
    buffer->slots[buffer->tail] = item;

    buffer->tail = (buffer->tail + 1) % buffer->capacity;
    buffer->size++;
    
    return 0;
}

buffer_t ring_buffer_get(ring_buffer_t *buffer){
    if(buffer == NULL || buffer->slots == NULL){
        LOG_WARN("cannot get NULL ring buffer");
        return(buffer_t){0};
    }
    if(buffer->size == 0){
        return (buffer_t){0};
    }
    buffer_t item = buffer->slots[buffer->head];

    buffer->slots[buffer->head] = (buffer_t){0};

    buffer->head = (buffer->head + 1) % buffer->capacity;
    buffer->size--;
    return item;
}
