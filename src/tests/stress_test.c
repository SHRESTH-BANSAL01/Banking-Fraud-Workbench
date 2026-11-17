
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#include "transaction.h"
#include "sorting.h"
#include "searching.h"
#include "stress_test.h"

typedef enum
{
    RANDOM_INPUT,
    SORTED_INPUT,
    REVERSE_INPUT,
    DUPLICATE_INPUT,
    IDENTICAL_INPUT
} StressType;

static uint32_t next_random(uint32_t *state)
{
    *state = (*state * 1664525u) + 1013904223u;
    return *state;
}

static const char *type_name(StressType type)
{
    switch (type)
    {
        case RANDOM_INPUT:
            return "Random";
        case SORTED_INPUT:
            return "Sorted";
        case REVERSE_INPUT:
            return "Reverse";
        case DUPLICATE_INPUT:
            return "Duplicates";
        case IDENTICAL_INPUT:
            return "Identical";
        default:
            return "Unknown";
    }
}

/* Generate deterministic test transactions */
static void generate_data(
    Transaction arr[],
    size_t n,
    StressType type)
{
    uint32_t seed = 123456u;

    for (size_t i = 0; i < n; i++)
    {
        Transaction t = {0};
        unsigned long long id = 0;

        switch (type)
        {
            case RANDOM_INPUT:
                id = next_random(&seed) % 1000000u + 1;
                break;

            case SORTED_INPUT:
                id = (unsigned long long)i + 1;
                break;

            case REVERSE_INPUT:
                id = (unsigned long long)(n - i);
                break;

            case DUPLICATE_INPUT:
                id = (unsigned long long)(i % 10) + 1;
                break;

            case IDENTICAL_INPUT:
                id = 1001;
                break;
        }

        t.transaction_id = id;
        t.account_id = 5000 + (id % 1000);
        t.amount = (double)(id % 10000);

        arr[i] = t;
    }
}

/* Independent comparison for reference sorting */
static int compare_ids(const void *a, const void *b)
{
    const Transaction *x = (const Transaction *)a;
    const Transaction *y = (const Transaction *)b;

    if (x->transaction_id < y->transaction_id)
        return -1;

    if (x->transaction_id > y->transaction_id)
        return 1;

    return 0;
}

/* Compare sorted transaction ID sequences */
static int same_ids(
    const Transaction a[],
    const Transaction b[],
    size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        if (a[i].transaction_id !=
            b[i].transaction_id)
        {
            return 0;
        }
    }

    return 1;
}

/* Verify that binary search finds an existing ID */
static int verify_binary_search(
    Transaction arr[],
    size_t n)
{
    if (n == 0)
        return 1;

    unsigned long long targets[] = {
        arr[0].transaction_id,
        arr[n / 2].transaction_id,
        arr[n - 1].transaction_id
    };

    for (size_t i = 0; i < 3; i++)
    {
        ptrdiff_t index = binary_search_transaction(
            arr,
            n,
            targets[i]
        );

        if (index < 0 ||
            (size_t)index >= n ||
            arr[index].transaction_id != targets[i])
        {
            return 0;
        }
    }

    return 1;
}

/* Execute one stress test */
static int execute_test(
    FILE *fp,
    size_t n,
    StressType type)
{
    Transaction *original = malloc(
        n * sizeof(Transaction)
    );

    Transaction *merge_arr = malloc(
        n * sizeof(Transaction)
    );

    Transaction *quick_arr = malloc(
        n * sizeof(Transaction)
    );

    Transaction *reference = malloc(
        n * sizeof(Transaction)
    );

    if (original == NULL ||
        merge_arr == NULL ||
        quick_arr == NULL ||
        reference == NULL)
    {
        printf("Memory allocation failed.\n");

        free(original);
        free(merge_arr);
        free(quick_arr);
        free(reference);

        return -1;
    }

    generate_data(original, n, type);

    memcpy(
        merge_arr,
        original,
        n * sizeof(Transaction)
    );

    memcpy(
        quick_arr,
        original,
        n * sizeof(Transaction)
    );

    memcpy(
        reference,
        original,
        n * sizeof(Transaction)
    );

    /* Independent reference result */
    qsort(
        reference,
        n,
        sizeof(Transaction),
        compare_ids
    );

    /* Test Merge Sort */
    int merge_success =
        merge_sort_transactions(merge_arr, n);

    /* Test Quick Sort */
    quick_sort_transactions(quick_arr, n);

    int merge_pass =
        merge_success &&
        same_ids(merge_arr, reference, n);

    int quick_pass =
        same_ids(quick_arr, reference, n);

    int binary_pass =
        merge_pass &&
        verify_binary_search(merge_arr, n);

    int overall =
        merge_pass && quick_pass && binary_pass;

    printf(
        "%-12s | %7zu | Merge:%s Quick:%s Search:%s\n",
        type_name(type),
        n,
        merge_pass ? "PASS" : "FAIL",
        quick_pass ? "PASS" : "FAIL",
        binary_pass ? "PASS" : "FAIL"
    );

    int written = fprintf(
        fp,
        "%zu,%s,%s,%s,%s,%s\n",
        n,
        type_name(type),
        merge_pass ? "PASS" : "FAIL",
        quick_pass ? "PASS" : "FAIL",
        binary_pass ? "PASS" : "FAIL",
        overall ? "PASS" : "FAIL"
    );

    free(original);
    free(merge_arr);
    free(quick_arr);
    free(reference);

    if (written < 0)
        return -1;

    return overall ? 1 : 0;
}

/* Main stress testing function */
int run_stress_tests(const char *output_path)
{
    if (output_path == NULL)
    {
        printf("Invalid stress test output path.\n");
        return 1;
    }

    const size_t sizes[] = {
        100, 1000, 5000, 10000, 20000
    };

    const StressType types[] = {
        RANDOM_INPUT,
        SORTED_INPUT,
        REVERSE_INPUT,
        DUPLICATE_INPUT,
        IDENTICAL_INPUT
    };

    FILE *fp = fopen(output_path, "w");

    if (fp == NULL)
    {
        printf("Unable to create stress test report.\n");
        printf("Check whether reports/ exists.\n");
        return 1;
    }

    if (fprintf(
            fp,
            "Dataset Size,Input Type,Merge Sort,"
            "Quick Sort,Binary Search,Overall\n"
        ) < 0)
    {
        fclose(fp);
        return 1;
    }

    printf("\n============================================\n");
    printf("         ALGORITHM STRESS TESTING\n");
    printf("============================================\n");

    int passed = 0;
    int failed = 0;

    size_t size_count =
        sizeof(sizes) / sizeof(sizes[0]);

    size_t type_count =
        sizeof(types) / sizeof(types[0]);

    for (size_t i = 0; i < size_count; i++)
    {
        for (size_t j = 0; j < type_count; j++)
        {
            int result = execute_test(
                fp,
                sizes[i],
                types[j]
            );

            if (result < 0)
            {
                printf("Stress test aborted.\n");
                fclose(fp);
                return 1;
            }

            if (result == 1)
                passed++;
            else
                failed++;
        }
    }

    if (fclose(fp) != 0)
    {
        printf("Failed to close stress test report.\n");
        return 1;
    }

    printf("\n============================================\n");
    printf("             STRESS TEST SUMMARY\n");
    printf("============================================\n");

    printf("Total Tests : %d\n", passed + failed);
    printf("Passed      : %d\n", passed);
    printf("Failed      : %d\n", failed);
    printf("CSV Report  : %s\n", output_path);

    printf("============================================\n");

    return failed == 0 ? 0 : 1;
}
