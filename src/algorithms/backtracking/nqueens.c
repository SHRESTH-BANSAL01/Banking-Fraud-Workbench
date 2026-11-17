
#include <stdio.h>
#include <stdlib.h>
#include "nqueens.h"

#define MAX_QUEENS 12

static int positions[MAX_QUEENS];
static int solution_count;

static int is_safe(int row, int col)
{
    for (int i = 0; i < row; i++) {
        int previous_col = positions[i];

        if (previous_col == col) {
            return 0;
        }

        if (abs(previous_col - col) == abs(i - row)) {
            return 0;
        }
    }

    return 1;
}

static void print_board(int n)
{
    printf("\nSolution %d:\n", solution_count);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (positions[i] == j) {
                printf("Q ");
            } else {
                printf(". ");
            }
        }
        printf("\n");
    }
}

static void backtrack(int row, int n)
{
    if (row == n) {
        solution_count++;

        if (solution_count <= 3) {
            print_board(n);
        }

        return;
    }

    for (int col = 0; col < n; col++) {
        if (is_safe(row, col)) {
            positions[row] = col;
            backtrack(row + 1, n);
        }
    }
}

void solve_nqueens(int n)
{
    if (n <= 0 || n > MAX_QUEENS) {
        printf("Invalid board size. Use 1 to %d.\n",
               MAX_QUEENS);
        return;
    }

    solution_count = 0;

    printf("\n============================================\n");
    printf("          N-QUEENS BACKTRACKING\n");
    printf("============================================\n");

    printf("Board Size: %d x %d\n", n, n);

    backtrack(0, n);

    printf("\nTotal Solutions: %d\n", solution_count);
    printf("============================================\n");
}
