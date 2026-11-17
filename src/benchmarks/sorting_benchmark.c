
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include "transaction.h"
#include "sorting.h"
#include "benchmark.h"

#define TRIALS 3

typedef enum
{
    INPUT_RANDOM,
    INPUT_SORTED,
    INPUT_REVERSE
} InputType;

/* Generate deterministic random numbers */
static uint32_t next_random(uint32_t *state)
{
    *state = (*state * 1664525u) + 1013904223u;
    return *state;
}

/* Return input type name */
static const char *input_name(InputType type)
{
    switch (type)
    {
        case INPUT_RANDOM:
            return "Random";

        case INPUT_SORTED:
            return "Sorted";

        case INPUT_REVERSE:
            return "Reverse";

        default:
            return "Unknown";
    }
}

/* Generate synthetic transaction dataset */
static void generate_transactions(
    Transaction *arr,
    size_t n,
    InputType type,
    unsigned int seed)
{
    uint32_t state = seed;

    for (size_t i = 0; i < n; i++)
    {
        Transaction t = {0};

        unsigned long long id;

        if (type == INPUT_SORTED)
        {
            id = (unsigned long long)i + 1;
        }
        else if (type == INPUT_REVERSE)
        {
            id = (unsigned long long)(n - i);
        }
        else
        {
            id = (unsigned long long)
                 (next_random(&state) % 1000000u) + 1;
        }

        t.transaction_id = id;
        t.account_id = 5000 + (id % 1000);
        t.device_id = 100 + (id % 100);
        t.merchant_id = 200 + (id % 100);
        t.amount = (double)(id % 100000) + 0.50;

        strcpy(
            t.timestamp,
            "2026-11-01T10:00:00"
        );

        t.is_fraud = 0;
        t.risk_score = 0;

        arr[i] = t;
    }
}

/* Verify ascending order */
static int verify_sorted(
    const Transaction *arr,
    size_t n)
{
    for (size_t i = 1; i < n; i++)
    {
        if (arr[i - 1].transaction_id >
            arr[i].transaction_id)
        {
            return 0;
        }
    }

    return 1;
}

/* Convert CPU clock ticks to milliseconds */
static double elapsed_ms(
    clock_t start,
    clock_t end)
{
    return ((double)(end - start) * 1000.0)
           / CLOCKS_PER_SEC;
}

/* Benchmark one sorting algorithm */
static int benchmark_one(
    FILE *fp,
    const Transaction *original,
    Transaction *working,
    size_t n,
    InputType type,
    int trial,
    int use_merge)
{
    memcpy(
        working,
        original,
        n * sizeof(Transaction)
    );

    clock_t start = clock();

    int success = 1;

    if (use_merge)
    {
        success = merge_sort_transactions(
            working,
            n
        );
    }
    else
    {
        quick_sort_transactions(
            working,
            n
        );
    }

    clock_t end = clock();

    int passed =
        success &&
        verify_sorted(working, n);

    const char *algorithm =
        use_merge ? "Merge Sort" : "Quick Sort";

    double milliseconds = elapsed_ms(
        start,
        end
    );

    printf(
        "%-12s | %7zu | %-7s | %d | %10.3f ms | %s\n",
        algorithm,
        n,
        input_name(type),
        trial,
        milliseconds,
        passed ? "PASS" : "FAIL"
    );

    if (fprintf(
            fp,
            "%s,%zu,%s,%d,%.6f,%s\n",
            algorithm,
            n,
            input_name(type),
            trial,
            milliseconds,
            passed ? "PASS" : "FAIL"
        ) < 0)
    {
        return 0;
    }

    return passed;
}

/* Main performance benchmarking function */
int run_sorting_benchmarks(
    const char *output_path)
{
    if (output_path == NULL)
    {
        printf("Invalid benchmark output path.\n");
        return 1;
    }

    const size_t sizes[] = {
        100,
        1000,
        10000,
        100000
    };

    const InputType types[] = {
        INPUT_RANDOM,
        INPUT_SORTED,
        INPUT_REVERSE
    };

    FILE *fp = fopen(
        output_path,
        "w"
    );

    if (fp == NULL)
    {
        printf(
            "Cannot create benchmark CSV: %s\n",
            output_path
        );

        printf(
            "Make sure the reports folder exists.\n"
        );

        return 1;
    }

    if (fprintf(
            fp,
            "Algorithm,Dataset Size,Input Type,Trial,"
            "Time (ms),Status\n"
        ) < 0)
    {
        printf("Failed to write CSV header.\n");
        fclose(fp);
        return 1;
    }

    printf("\n");
    printf("============================================\n");
    printf("       SORTING PERFORMANCE BENCHMARK\n");
    printf("============================================\n");

    printf(
        "Algorithm    |    Size | Input   | T | "
        "Time          | Status\n"
    );

    printf(
        "------------------------------------------------------------\n"
    );

    int failures = 0;
    int total_runs = 0;

    size_t size_count =
        sizeof(sizes) / sizeof(sizes[0]);

    size_t type_count =
        sizeof(types) / sizeof(types[0]);

    for (size_t s = 0; s < size_count; s++)
    {
        size_t n = sizes[s];

        Transaction *original = malloc(
            n * sizeof(Transaction)
        );

        Transaction *working = malloc(
            n * sizeof(Transaction)
        );

        if (original == NULL ||
            working == NULL)
        {
            printf(
                "Memory allocation failed for %zu records.\n",
                n
            );

            free(original);
            free(working);
            fclose(fp);

            return 1;
        }

        for (size_t t = 0; t < type_count; t++)
        {
            for (int trial = 1;
                 trial <= TRIALS;
                 trial++)
            {
                generate_transactions(
                    original,
                    n,
                    types[t],
                    (unsigned int)(
                        12345 +
                        s * 100 +
                        t * 10 +
                        trial
                    )
                );

                int merge_pass = benchmark_one(
                    fp,
                    original,
                    working,
                    n,
                    types[t],
                    trial,
                    1
                );

                int quick_pass = benchmark_one(
                    fp,
                    original,
                    working,
                    n,
                    types[t],
                    trial,
                    0
                );

                total_runs += 2;

                if (!merge_pass)
                {
                    failures++;
                }

                if (!quick_pass)
                {
                    failures++;
                }
            }
        }

        free(original);
        free(working);
    }

    if (fclose(fp) != 0)
    {
        printf("Failed to close benchmark CSV.\n");
        return 1;
    }

    printf("\n");
    printf("============================================\n");
    printf("             BENCHMARK SUMMARY\n");
    printf("============================================\n");

    printf(
        "Total Benchmark Runs : %d\n",
        total_runs
    );

    printf(
        "Passed               : %d\n",
        total_runs - failures
    );

    printf(
        "Failed               : %d\n",
        failures
    );

    printf(
        "CSV Report           : %s\n",
        output_path
    );

    printf("============================================\n");

    return failures == 0 ? 0 : 1;
}
