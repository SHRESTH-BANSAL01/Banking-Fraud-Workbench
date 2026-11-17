
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

#include "branch_bound.h"

typedef struct {
    InvestigationItem item;
    size_t original_index;
} BBItem;

typedef struct {
    const BBItem *items;
    size_t n;
    int budget;
    int best_value;
    int best_cost;
    unsigned char *current;
    unsigned char *best;
    unsigned long long nodes_visited;
    unsigned long long nodes_pruned;
} BBContext;

/* Sort by descending value-to-cost ratio */
static int compare_ratio(const void *a, const void *b)
{
    const BBItem *x = (const BBItem *)a;
    const BBItem *y = (const BBItem *)b;

    double rx = (double)x->item.value / x->item.cost;
    double ry = (double)y->item.value / y->item.cost;

    if (rx > ry) return -1;
    if (rx < ry) return 1;
    return 0;
}

/* Fractional knapsack upper bound */
static double calculate_bound(
    const BBContext *ctx,
    size_t level,
    int cost,
    int value)
{
    double bound = value;
    int remaining = ctx->budget - cost;

    for (size_t i = level; i < ctx->n && remaining > 0; i++) {
        int item_cost = ctx->items[i].item.cost;
        int item_value = ctx->items[i].item.value;

        if (item_cost <= remaining) {
            bound += item_value;
            remaining -= item_cost;
        } else {
            bound += (double)item_value *
                     remaining / item_cost;
            break;
        }
    }

    return bound;
}

/* Explore include/exclude decisions */
static void search(
    BBContext *ctx,
    size_t level,
    int cost,
    int value)
{
    ctx->nodes_visited++;

    if (value > ctx->best_value) {
        ctx->best_value = value;
        ctx->best_cost = cost;

        for (size_t i = 0; i < ctx->n; i++) {
            ctx->best[i] = ctx->current[i];
        }
    }

    if (level == ctx->n) {
        return;
    }

    double bound = calculate_bound(ctx, level, cost, value);

    if (bound + 1e-9 < ctx->best_value) {
        ctx->nodes_pruned++;
        return;
    }

    int item_cost = ctx->items[level].item.cost;
    int item_value = ctx->items[level].item.value;

    /* Include the current investigation */
    if (item_cost <= ctx->budget - cost &&
        item_value <= INT_MAX - value) {

        ctx->current[level] = 1;

        search(ctx,
               level + 1,
               cost + item_cost,
               value + item_value);
    }

    /* Exclude the current investigation */
    ctx->current[level] = 0;

    search(ctx, level + 1, cost, value);
}

void branch_bound_knapsack(
    const InvestigationItem items[],
    size_t n,
    int budget)
{
    if (items == NULL || n == 0 || budget < 0) {
        printf("Invalid Branch and Bound input.\n");
        return;
    }

    if (n > SIZE_MAX / sizeof(BBItem)) {
        printf("Too many investigations.\n");
        return;
    }

    for (size_t i = 0; i < n; i++) {
        if (items[i].cost <= 0 || items[i].value < 0) {
            printf("Costs must be positive and values non-negative.\n");
            return;
        }
    }

    BBItem *sorted = malloc(n * sizeof(BBItem));
    unsigned char *current = calloc(n, sizeof(unsigned char));
    unsigned char *best = calloc(n, sizeof(unsigned char));

    if (sorted == NULL || current == NULL || best == NULL) {
        printf("Memory allocation failed.\n");
        free(sorted);
        free(current);
        free(best);
        return;
    }

    for (size_t i = 0; i < n; i++) {
        sorted[i].item = items[i];
        sorted[i].original_index = i;
    }

    qsort(sorted, n, sizeof(BBItem), compare_ratio);

    BBContext ctx = {
        sorted,
        n,
        budget,
        0,
        0,
        current,
        best,
        0,
        0
    };

    search(&ctx, 0, 0, 0);

    printf("\n============================================\n");
    printf("       BRANCH AND BOUND - KNAPSACK\n");
    printf("============================================\n");

    printf("Investigation Budget: %d\n", budget);
    printf("Maximum Priority Value: %d\n", ctx.best_value);

    printf("\nSelected Investigations:\n");

    for (size_t i = 0; i < n; i++) {
        if (ctx.best[i]) {
            printf("Transaction %llu | Cost: %d | Value: %d\n",
                   sorted[i].item.transaction_id,
                   sorted[i].item.cost,
                   sorted[i].item.value);
        }
    }

    printf("\nTotal Investigation Cost: %d\n", ctx.best_cost);
    printf("Remaining Budget: %d\n", budget - ctx.best_cost);

    printf("\nSearch Statistics:\n");
    printf("Nodes Visited: %llu\n", ctx.nodes_visited);
    printf("Nodes Pruned: %llu\n", ctx.nodes_pruned);

    printf("============================================\n");

    free(sorted);
    free(current);
    free(best);
}
