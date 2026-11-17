
#ifndef MAX_FLOW_H
#define MAX_FLOW_H

#include "graph.h"

void edmonds_karp(
    int capacity[MAX_NODES][MAX_NODES],
    int n,
    int source,
    int sink
);

#endif
