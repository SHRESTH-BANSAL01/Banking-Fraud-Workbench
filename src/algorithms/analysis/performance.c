
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "performance.h"
#include "sorting.h"

void compare_sorting_algorithms(Transaction *arr, size_t n)
{
    if (arr == NULL || n == 0) {
        printf("No transactions available.\n");
        return;
    }

    Transaction *merge_arr = malloc(n * sizeof(Transaction));
    Transaction *quick_arr = malloc(n * sizeof(Transaction));

    if (merge_arr == NULL || quick_arr == NULL) {
        printf("Memory allocation failed.\n");
        free(merge_arr);
        free(quick_arr);
        return;
    }

    memcpy(merge_arr, arr, n * sizeof(Transaction));
    memcpy(quick_arr, arr, n * sizeof(Transaction));

    clock_t start, end;
    double merge_time, quick_time;

    printf("\n============================================\n");
    printf("       SORTING PERFORMANCE ANALYSIS\n");
    printf("============================================\n");

    start = clock();
    int merge_success = merge_sort_transactions(merge_arr, n);
    end = clock();

    if (!merge_success) {
        printf("Merge Sort failed.\n");
        free(merge_arr);
        free(quick_arr);
        return;
    }

    merge_time = (double)(end - start) / CLOCKS_PER_SEC;

    start = clock();
    quick_sort_transactions(quick_arr, n);
    end = clock();

    quick_time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Total Transactions : %zu\n", n);
    printf("Merge Sort Time    : %.6f seconds\n", merge_time);
    printf("Quick Sort Time    : %.6f seconds\n", quick_time);

    printf("\nTheoretical Time Complexity:\n");
    printf("Merge Sort : O(n log n)\n");
    printf("Quick Sort : O(n log n) average\n");
    printf("Quick Sort : O(n^2) worst case\n");

    printf("============================================\n");

    free(merge_arr);
    free(quick_arr);
}
