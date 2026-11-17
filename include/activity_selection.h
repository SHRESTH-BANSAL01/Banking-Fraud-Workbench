
#ifndef ACTIVITY_SELECTION_H
#define ACTIVITY_SELECTION_H

#include <stddef.h>

typedef struct {
    unsigned long long transaction_id;
    int start_time;
    int end_time;
} Investigation;

void schedule_investigations(Investigation arr[], size_t n);

#endif
