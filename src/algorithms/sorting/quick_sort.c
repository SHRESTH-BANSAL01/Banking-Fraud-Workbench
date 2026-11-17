
#include <stddef.h>

#include "sorting.h"

// Swap two transactions
static void swap(Transaction *a, Transaction *b)
{
    Transaction temp = *a;

    *a = *b;

    *b = temp;
}

// Recursive Quick Sort
static void quick_sort_recursive(Transaction arr[],
                                 ptrdiff_t low,
                                 ptrdiff_t high)
{
    if (low >= high)
    {
        return;
    }

    // Select middle element as pivot
    unsigned long long pivot =
        arr[low + (high - low) / 2].transaction_id;

    ptrdiff_t i = low;
    ptrdiff_t j = high;

    while (i <= j)
    {
        while (arr[i].transaction_id < pivot)
        {
            i++;
        }

        while (arr[j].transaction_id > pivot)
        {
            j--;
        }

        if (i <= j)
        {
            swap(&arr[i], &arr[j]);

            i++;
            j--;
        }
    }

    // Sort left partition
    if (low < j)
    {
        quick_sort_recursive(arr, low, j);
    }

    // Sort right partition
    if (i < high)
    {
        quick_sort_recursive(arr, i, high);
    }
}

// Public Quick Sort function
void quick_sort_transactions(Transaction *arr, size_t n)
{
    if (arr == NULL || n < 2)
    {
        return;
    }

    quick_sort_recursive(arr, 0, (ptrdiff_t)n - 1);
}
