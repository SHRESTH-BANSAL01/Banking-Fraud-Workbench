
#ifndef SEARCHING_H
#define SEARCHING_H

#include <stddef.h>
#include "transaction.h"

// Returns index if found, otherwise -1
ptrdiff_t binary_search_transaction(
    Transaction *arr,
    size_t n,
    unsigned long long target
);

#endif
