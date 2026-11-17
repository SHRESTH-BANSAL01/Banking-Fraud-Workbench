
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#include "knapsack.h"
#include "comparison.h"

#define COMPARISON_TRIALS 5

typedef struct
{
    int cost;
    int value;
} CompareItem;

typedef struct
{
    CompareItem *items;
    size_t n;
    int budget;
    int best_value;
    unsigned long long nodes_visited;
    unsigned long long nodes_pruned;
} BBState;

/* Compare items by descending value/cost ratio */
static int compare_ratio(const void *a, const void *b)
{
    const CompareItem *x = (const CompareItem *)a;
    const CompareItem *y = (const CompareItem *)b;

    long long left =
        (long long)x->value * y->cost;

    long long right =
        (long long)y->value * x->cost;

    if (left > right)
        return -1;

    if (left < right)
        return 1;

    return 0;
}

/* Dynamic Programming with one-dimensional table */
static int solve_dp(
    const InvestigationItem items[],
    size_t n,
    int budget)
{
    int *dp = calloc(
        (size_t)budget + 1,
        sizeof(int)
    );

    if (dp == NULL)
        return -1;

    for (size_t i = 0; i < n; i++)
    {
        int cost = items[i].cost;
        int value = items[i].value;

        for (int w = budget; w >= cost; w--)
        {
            int candidate = dp[w - cost] + value;

            if (candidate > dp[w])
                dp[w] = candidate;
        }
    }

    int result = dp[budget];

    free(dp);
    return result;
}

/* Fractional upper bound for Branch and Bound */
static double calculate_bound(
    const BBState *state,
    size_t level,
    int cost,
    int value)
{
    double bound = (double)value;
    int remaining = state->budget - cost;

    for (size_t i = level;
         i < state->n && remaining > 0;
         i++)
    {
        int item_cost = state->items[i].cost;
        int item_value = state->items[i].value;

        if (item_cost <= remaining)
        {
            bound += item_value;
            remaining -= item_cost;
        }
        else
        {
            bound +=
                (double)item_value *
                remaining / item_cost;
            break;
        }
    }

    return bound;
}

/* Recursive Branch and Bound search */
static void bb_search(
    BBState *state,
    size_t level,
    int cost,
    int value)
{
    state->nodes_visited++;

    if (value > state->best_value)
        state->best_value = value;

    if (level == state->n)
        return;

    double bound = calculate_bound(
        state,
        level,
        cost,
        value
    );

    if (bound + 1e-9 < state->best_value)
    {
        state->nodes_pruned++;
        return;
    }

    CompareItem item = state->items[level];

    /* Include current investigation */
    if (item.cost <= state->budget - cost &&
        item.value <= INT_MAX - value)
    {
        bb_search(
            state,
            level + 1,
            cost + item.cost,
            value + item.value
        );
    }

    /* Exclude current investigation */
    bb_search(
        state,
        level + 1,
        cost,
        value
    );
}

/* Solve Branch and Bound */
static int solve_bb(
    const InvestigationItem items[],
    size_t n,
    int budget,
    unsigned long long *visited,
    unsigned long long *pruned)
{
    CompareItem *sorted = malloc(
        n * sizeof(CompareItem)
    );

    if (sorted == NULL)
        return -1;

    for (size_t i = 0; i < n; i++)
    {
        sorted[i].cost = items[i].cost;
        sorted[i].value = items[i].value;
    }

    qsort(
        sorted,
        n,
        sizeof(CompareItem),
        compare_ratio
    );

    BBState state = {
        sorted,
        n,
        budget,
        0,
        0,
        0
    };

    bb_search(&state, 0, 0, 0);

    *visited = state.nodes_visited;
    *pruned = state.nodes_pruned;

    int result = state.best_value;

    free(sorted);
    return result;
}

/* Generate deterministic investigation data */
static void generate_items(
    InvestigationItem items[],
    size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        items[i].transaction_id =
            1000ULL + (unsigned long long)i;

        items[i].cost = 1 + (int)(i % 10);

        items[i].value =
            10 + (int)((i * 17) % 91);
    }
}

/* Convert CPU ticks to milliseconds */
static double elapsed_ms(
    clock_t start,
    clock_t end)
{
    return 1000.0 *
           (double)(end - start) /
           CLOCKS_PER_SEC;
}

int run_algorithm_comparison(
    const char *output_path)
{
    if (output_path == NULL)
    {
        printf("Invalid comparison report path.\n");
        return 1;
    }

    const size_t sizes[] = {
        5, 10, 15, 20, 25
    };

    const int budget = 30;

    FILE *fp = fopen(output_path, "w");

    if (fp == NULL)
    {
        printf("Cannot create comparison CSV.\n");
        printf("Make sure reports/ exists.\n");
        return 1;
    }

    if (fprintf(
            fp,
            "Items,Budget,Trial,DP Value,BB Value,"
            "DP Time (ms),BB Time (ms),"
            "Nodes Visited,Nodes Pruned,Status\n"
        ) < 0)
    {
        fclose(fp);
        return 1;
    }

    printf("\n============================================\n");
    printf("       CROSS-PARADIGM COMPARISON\n");
    printf("============================================\n");

    printf("Dynamic Programming vs Branch and Bound\n");
    printf("Budget: %d\n\n", budget);

    int total = 0;
    int passed = 0;
    int failed = 0;

    size_t size_count =
        sizeof(sizes) / sizeof(sizes[0]);

    for (size_t s = 0; s < size_count; s++)
    {
        size_t n = sizes[s];

        InvestigationItem *items = malloc(
            n * sizeof(InvestigationItem)
        );

        if (items == NULL)
        {
            printf("Memory allocation failed.\n");
            fclose(fp);
            return 1;
        }

        generate_items(items, n);

        for (int trial = 1;
             trial <= COMPARISON_TRIALS;
             trial++)
        {
            clock_t dp_start = clock();

            int dp_value = solve_dp(
                items,
                n,
                budget
            );

            clock_t dp_end = clock();

            unsigned long long visited = 0;
            unsigned long long pruned = 0;

            clock_t bb_start = clock();

            int bb_value = solve_bb(
                items,
                n,
                budget,
                &visited,
                &pruned
            );

            clock_t bb_end = clock();

            double dp_time =
                elapsed_ms(dp_start, dp_end);

            double bb_time =
                elapsed_ms(bb_start, bb_end);

            int success =
                dp_value >= 0 &&
                bb_value >= 0 &&
                dp_value == bb_value;

            total++;

            if (success)
                passed++;
            else
                failed++;

            printf(
                "Items: %-3zu | Trial: %d | "
                "DP: %-4d | BB: %-4d | %s\n",
                n,
                trial,
                dp_value,
                bb_value,
                success ? "PASS" : "FAIL"
            );

            if (fprintf(
                    fp,
                    "%zu,%d,%d,%d,%d,%.6f,%.6f,"
                    "%llu,%llu,%s\n",
                    n,
                    budget,
                    trial,
                    dp_value,
                    bb_value,
                    dp_time,
                    bb_time,
                    visited,
                    pruned,
                    success ? "PASS" : "FAIL"
                ) < 0)
            {
                printf("Failed to write comparison CSV.\n");
                free(items);
                fclose(fp);
                return 1;
            }
        }

        free(items);
    }

    if (fclose(fp) != 0)
    {
        printf("Failed to close comparison CSV.\n");
        return 1;
    }

    printf("\n============================================\n");
    printf("           COMPARISON SUMMARY\n");
    printf("============================================\n");

    printf("Total Comparisons : %d\n", total);
    printf("Passed            : %d\n", passed);
    printf("Failed            : %d\n", failed);
    printf("CSV Report        : %s\n", output_path);

    printf("============================================\n");

    return failed == 0 ? 0 : 1;
}
