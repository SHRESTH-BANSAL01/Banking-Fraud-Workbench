
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#include "mcm.h"

static void print_parenthesization(
    const size_t *split,
    size_t cols,
    size_t i,
    size_t j)
{
    if (i == j) {
        printf("A%zu", i + 1);
        return;
    }

    size_t k = split[i * cols + j];

    printf("(");
    print_parenthesization(split, cols, i, k);
    printf(" x ");
    print_parenthesization(split, cols, k + 1, j);
    printf(")");
}

void matrix_chain_multiplication(
    const int dimensions[],
    size_t matrix_count)
{
    if (dimensions == NULL || matrix_count == 0) {
        printf("Invalid matrix chain.\n");
        return;
    }

    for (size_t i = 0; i <= matrix_count; i++) {
        if (dimensions[i] <= 0) {
            printf("Matrix dimensions must be positive.\n");
            return;
        }
    }

    if (matrix_count > SIZE_MAX / matrix_count ||
        matrix_count * matrix_count > SIZE_MAX / sizeof(long long) ||
        matrix_count * matrix_count > SIZE_MAX / sizeof(size_t)) {
        printf("Matrix chain is too large.\n");
        return;
    }

    size_t cells = matrix_count * matrix_count;

    long long *dp = malloc(cells * sizeof(long long));
    size_t *split = calloc(cells, sizeof(size_t));

    if (dp == NULL || split == NULL) {
        printf("Memory allocation failed.\n");
        free(dp);
        free(split);
        return;
    }

    for (size_t i = 0; i < cells; i++) {
        dp[i] = LLONG_MAX;
    }

    #define DP(i, j) dp[(i) * matrix_count + (j)]
    #define SPLIT(i, j) split[(i) * matrix_count + (j)]

    for (size_t i = 0; i < matrix_count; i++) {
        DP(i, i) = 0;
    }

    for (size_t len = 2; len <= matrix_count; len++) {
        for (size_t i = 0; i <= matrix_count - len; i++) {
            size_t j = i + len - 1;

            for (size_t k = i; k < j; k++) {
                long long a = dimensions[i];
                long long b = dimensions[k + 1];
                long long c = dimensions[j + 1];

                if (a > LLONG_MAX / b) continue;
                long long ab = a * b;

                if (ab > LLONG_MAX / c) continue;
                long long multiply_cost = ab * c;

                if (DP(i, k) == LLONG_MAX ||
                    DP(k + 1, j) == LLONG_MAX) {
                    continue;
                }

                if (DP(i, k) > LLONG_MAX - DP(k + 1, j)) {
                    continue;
                }

                long long partial = DP(i, k) + DP(k + 1, j);

                if (partial > LLONG_MAX - multiply_cost) {
                    continue;
                }

                long long cost = partial + multiply_cost;

                if (cost < DP(i, j)) {
                    DP(i, j) = cost;
                    SPLIT(i, j) = k;
                }
            }
        }
    }

    printf("\n============================================\n");
    printf("       MATRIX CHAIN MULTIPLICATION\n");
    printf("============================================\n");

    printf("Number of Matrices: %zu\n", matrix_count);

    if (DP(0, matrix_count - 1) == LLONG_MAX) {
        printf("Unable to calculate cost within integer limits.\n");
    } else {
        printf("Minimum Scalar Multiplications: %lld\n",
               DP(0, matrix_count - 1));

        printf("Optimal Parenthesization: ");
        print_parenthesization(
            split, matrix_count, 0, matrix_count - 1
        );
        printf("\n");
    }

    printf("============================================\n");

    #undef DP
    #undef SPLIT

    free(dp);
    free(split);
}
