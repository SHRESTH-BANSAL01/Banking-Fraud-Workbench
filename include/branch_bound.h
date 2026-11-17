
#ifndef BRANCH_BOUND_H
#define BRANCH_BOUND_H

#include <stddef.h>
#include "knapsack.h"

void branch_bound_knapsack(
    const InvestigationItem items[],
    size_t n,
    int budget
);

#endif
