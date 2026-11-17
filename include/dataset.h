
#ifndef DATASET_H
#define DATASET_H

#include <stddef.h>
#include "transaction.h"

typedef struct Dataset
{
    Transaction *items;
    size_t count;

} Dataset;

int load_dataset(const char *filename, Dataset *dataset);

void free_dataset(Dataset *dataset);

#endif
