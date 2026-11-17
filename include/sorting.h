
#ifndef SORTING_H
#define SORTING_H

#include <stddef.h>
#include "transaction.h"

// Merge Sort
int merge_sort_transactions(Transaction *arr, size_t n);

// Quick Sort
void quick_sort_transactions(Transaction *arr, size_t n);

#endif
