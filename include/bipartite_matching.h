
#ifndef BIPARTITE_MATCHING_H
#define BIPARTITE_MATCHING_H

#define MAX_CASES 20
#define MAX_INVESTIGATORS 20

void assign_investigators(
    int compatibility[MAX_CASES][MAX_INVESTIGATORS],
    int cases,
    int investigators,
    const unsigned long long transaction_ids[]
);

#endif
