#ifndef NETFORGE_SCHEDULE_SCHEDULE_H
#define NETFORGE_SCHEDULE_SCHEDULE_H

#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include "../logger/logger.h"

typedef void (*event_callback_t)(void *ctx);

typedef struct{
    time_t execute_at;
    event_callback_t callback;
    void *ctx;
} event_t;

typedef struct{
    event_t *events;
    size_t capacity;
    size_t size;
} event_queue_t;

int event_queue_init(event_queue_t *event_queue);
void event_queue_destroy(event_queue_t *event_queue);

int schedule_add_event(event_queue_t *event_queue, event_t new_event);
void schedule_check_events(event_queue_t *event_queue);

time_t simulator_now();

#endif //NETFORGE_SCHEDULE_SCHEDULE_H