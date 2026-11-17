
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "lcs.h"

void compare_transaction_patterns(
    const char *pattern1,
    const char *pattern2)
{
    if (pattern1 == NULL || pattern2 == NULL) {
        printf("Invalid transaction patterns.\n");
        return;
    }

    size_t m = strlen(pattern1);
    size_t n = strlen(pattern2);

    if (m == SIZE_MAX || n == SIZE_MAX ||
        m + 1 > SIZE_MAX / (n + 1) ||
        (m + 1) * (n + 1) > SIZE_MAX / sizeof(size_t)) {
        printf("LCS table is too large.\n");
        return;
    }

    size_t cols = n + 1;

    size_t *dp = calloc((m + 1) * cols, sizeof(size_t));

    if (dp == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    #define DP(i, j) dp[(i) * cols + (j)]

    for (size_t i = 1; i <= m; i++) {
        for (size_t j = 1; j <= n; j++) {
            if (pattern1[i - 1] == pattern2[j - 1]) {
                DP(i, j) = 1 + DP(i - 1, j - 1);
            } else {
                size_t top = DP(i - 1, j);
                size_t left = DP(i, j - 1);

                DP(i, j) = top > left ? top : left;
            }
        }
    }

    size_t length = DP(m, n);

    char *lcs = malloc(length + 1);

    if (lcs == NULL) {
        printf("Memory allocation failed.\n");
        free(dp);
        return;
    }

    lcs[length] = '\0';

    size_t i = m;
    size_t j = n;
    size_t k = length;

    while (i > 0 && j > 0) {
        if (pattern1[i - 1] == pattern2[j - 1]) {
            lcs[--k] = pattern1[i - 1];
            i--;
            j--;
        } else if (DP(i - 1, j) >= DP(i, j - 1)) {
            i--;
        } else {
            j--;
        }
    }

    printf("\n============================================\n");
    printf("      LONGEST COMMON SUBSEQUENCE\n");
    printf("============================================\n");

    printf("Pattern 1: %s\n", pattern1);
    printf("Pattern 2: %s\n", pattern2);

    printf("\nLongest Common Subsequence: %s\n", lcs);
    printf("LCS Length: %zu\n", length);

    size_t max_length = m > n ? m : n;

    double similarity = max_length == 0
        ? 100.0
        : (double)length / max_length * 100.0;

    printf("Similarity Score: %.2f%%\n", similarity);

    printf("============================================\n");

    #undef DP

    free(lcs);
    free(dp);
}
