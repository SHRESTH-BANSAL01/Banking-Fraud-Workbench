
#include <stdio.h>
#include <stdlib.h>
#include "knapsack.h"

void knapsack_investigations(
    const InvestigationItem items[],
    size_t n,
    int budget)
{
    if (items == NULL || n == 0 || budget < 0) {
        printf("Invalid knapsack input.\n");
        return;
    }

    for (size_t i = 0; i < n; i++) {
        if (items[i].cost < 0 || items[i].value < 0) {
            printf("Invalid investigation cost or value.\n");
            return;
        }
    }

    size_t cols = (size_t)budget + 1;

    if (n == (size_t)-1 ||
        cols == 0 ||
        n + 1 > (size_t)-1 / cols ||
        (n + 1) * cols > (size_t)-1 / sizeof(int)) {
        printf("DP table is too large.\n");
        return;
    }

    int *dp = calloc((n + 1) * cols, sizeof(int));

    if (dp == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    #define DP(i, w) dp[(i) * cols + (w)]

    for (size_t i = 1; i <= n; i++) {
        int cost = items[i - 1].cost;
        int value = items[i - 1].value;

        for (int w = 0; w <= budget; w++) {
            DP(i, w) = DP(i - 1, w);

            if (cost <= w) {
                int previous = DP(i - 1, w - cost);

                if (value <= 2147483647 - previous) {
                    int include = value + previous;

                    if (include > DP(i, w)) {
                        DP(i, w) = include;
                    }
                }
            }
        }
    }

    printf("\n============================================\n");
    printf("        0/1 KNAPSACK - FRAUD ANALYSIS\n");
    printf("============================================\n");

    printf("Investigation Budget: %d\n", budget);
    printf("Maximum Priority Value: %d\n", DP(n, budget));

    printf("\nSelected Investigations:\n");

    size_t i = n;
    int w = budget;
    int total_cost = 0;

    while (i > 0) {
        if (DP(i, w) != DP(i - 1, w)) {
            printf("Transaction %llu | Cost: %d | Value: %d\n",
                   items[i - 1].transaction_id,
                   items[i - 1].cost,
                   items[i - 1].value);

            total_cost += items[i - 1].cost;
            w -= items[i - 1].cost;
        }

        i--;
    }

    printf("\nTotal Investigation Cost: %d\n", total_cost);
    printf("Remaining Budget: %d\n", budget - total_cost);
    printf("============================================\n");

    #undef DP

    free(dp);
}
