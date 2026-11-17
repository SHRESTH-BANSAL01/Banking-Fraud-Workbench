
#ifndef KNAPSACK_H
#define KNAPSACK_H

#include <stddef.h>

typedef struct {
    unsigned long long transaction_id;
    int cost;
    int value;
} InvestigationItem;

void knapsack_investigations(
    const InvestigationItem items[],
    size_t n,
    int budget
);

#endif
