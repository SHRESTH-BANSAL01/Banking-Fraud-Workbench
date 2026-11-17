
#include <stdio.h>
#include <stdlib.h>
#include "activity_selection.h"

static int compare_end_time(const void *a, const void *b)
{
    const Investigation *x = (const Investigation *)a;
    const Investigation *y = (const Investigation *)b;

    if (x->end_time < y->end_time) return -1;
    if (x->end_time > y->end_time) return 1;
    return 0;
}

void schedule_investigations(Investigation arr[], size_t n)
{
    if (arr == NULL || n == 0) {
        printf("No investigations available.\n");
        return;
    }

    Investigation *sorted = malloc(n * sizeof(Investigation));

    if (sorted == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    for (size_t i = 0; i < n; i++) {
        sorted[i] = arr[i];
    }

    qsort(sorted, n, sizeof(Investigation), compare_end_time);

    printf("\n============================================\n");
    printf("       GREEDY INVESTIGATION SCHEDULING\n");
    printf("============================================\n");

    int last_end = sorted[0].end_time;
    int selected = 1;

    printf("Selected Transaction: %llu\n",
           sorted[0].transaction_id);

    for (size_t i = 1; i < n; i++) {
        if (sorted[i].start_time >= last_end) {
            printf("Selected Transaction: %llu\n",
                   sorted[i].transaction_id);

            last_end = sorted[i].end_time;
            selected++;
        }
    }

    printf("\nTotal Investigations Selected: %d\n", selected);
    printf("============================================\n");

    free(sorted);
}
