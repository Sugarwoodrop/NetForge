#include "../../include/netforge/schedule/schedule.h"

time_t simulator_now(){
    static time_t start_time = 0;
    if (start_time == 0){
        start_time = time(NULL);
    }
    return time(NULL) - start_time;
}

int event_queue_init(event_queue_t *event_queue){
    if (event_queue == NULL) {
        LOG_WARN("cannot initialize NULL event_queue");
        return -1;
    }
    event_queue->capacity = 30;
    event_queue->size = 0;
    event_queue->events = malloc(sizeof(event_t) * event_queue->capacity);
    if(event_queue->events == NULL){
        LOG_ERROR("failed to allocate memory for event_queue");
        return -1;
    }
    return 0;
}

void event_queue_destroy(event_queue_t *event_queue){
    if (event_queue == NULL || event_queue->events == NULL) {
        LOG_WARN("cannot destroy NULL event_queue");
        return;
    }
    free(event_queue->events);
    event_queue->capacity = 0;
    event_queue->size = 0;
}

int schedule_add_event(event_queue_t *event_queue, event_t new_event){
    if (event_queue == NULL || event_queue->events == NULL) {
        LOG_WARN("cannot add even in NULL event_queue");
        return -1;
    }
    new_event.execute_at += simulator_now();
    if(event_queue->size >= event_queue->capacity){
        event_t *buf = realloc(event_queue->events, sizeof(event_t) * event_queue->capacity*2);
        if(buf == NULL){
            LOG_ERROR("failed to allocate memory for event_t buffer");
            return -1;
        }
        event_queue->events = buf;
        event_queue->capacity *= 2;
    }
    event_queue->events[event_queue->size] = new_event;
    event_queue->size++;
    return 0;
}

void schedule_check_events(event_queue_t *event_queue){
    if (event_queue == NULL || event_queue->events == NULL) {
        LOG_WARN("cannot check NULL event_queue");
        return;
    }

    time_t time_now = simulator_now();
    size_t i = 0;

    while (i < event_queue->size) {
        if(event_queue->events[i].execute_at <= time_now){
            if (event_queue->events[i].callback != NULL){
                event_queue->events[i].callback(event_queue->events[i].ctx);
            }
            event_queue->events[i] = event_queue->events[event_queue->size - 1];
            event_queue->events[event_queue->size - 1] = (event_t){0};
            event_queue->size--;
            continue;
        } 
        i++;
    }
}