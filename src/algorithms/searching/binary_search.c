
#include "searching.h"

ptrdiff_t binary_search_transaction(
    Transaction *arr,
    size_t n,
    unsigned long long target)
{
    if (arr == NULL || n == 0) {
        return -1;
    }

    ptrdiff_t left = 0;
    ptrdiff_t right = (ptrdiff_t)n - 1;

    while (left <= right) {
        ptrdiff_t mid = left + (right - left) / 2;

        unsigned long long current = arr[mid].transaction_id;

        if (current == target) {
            return mid;
        }

        if (current < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;
}
