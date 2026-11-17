
#include <stdio.h>
#include <stdlib.h>

#include "sorting.h"

// Merge two sorted portions
static void merge(Transaction arr[],
                  Transaction temp[],
                  size_t left,
                  size_t mid,
                  size_t right)
{
    size_t i = left;
    size_t j = mid + 1;
    size_t k = left;

    // Compare and merge
    while (i <= mid && j <= right)
    {
        if (arr[i].transaction_id <=
            arr[j].transaction_id)
        {
            temp[k++] = arr[i++];
        }
        else
        {
            temp[k++] = arr[j++];
        }
    }

    // Copy remaining left elements
    while (i <= mid)
    {
        temp[k++] = arr[i++];
    }

    // Copy remaining right elements
    while (j <= right)
    {
        temp[k++] = arr[j++];
    }

    // Copy merged elements back
    for (i = left; i <= right; i++)
    {
        arr[i] = temp[i];
    }
}

// Recursive Merge Sort
static void merge_sort_recursive(Transaction arr[],
                                 Transaction temp[],
                                 size_t left,
                                 size_t right)
{
    if (left >= right)
    {
        return;
    }

    size_t mid = left + (right - left) / 2;

    merge_sort_recursive(arr, temp, left, mid);

    merge_sort_recursive(arr, temp, mid + 1, right);

    merge(arr, temp, left, mid, right);
}

// Public Merge Sort function
int merge_sort_transactions(Transaction *arr, size_t n)
{
    if (arr == NULL && n > 0)
    {
        return 0;
    }

    if (n < 2)
    {
        return 1;
    }

    Transaction *temp = malloc(n * sizeof(Transaction));

    if (temp == NULL)
    {
        printf("Merge Sort: Memory allocation failed.\n");
        return 0;
    }

    merge_sort_recursive(arr, temp, 0, n - 1);

    free(temp);

    return 1;
}
