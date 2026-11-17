
#include <stdio.h>
#include <stddef.h>

#include "complexity_report.h"

typedef struct
{
    const char *algorithm;
    const char *paradigm;
    const char *best;
    const char *average;
    const char *worst;
    const char *space;
    const char *classification;
} ComplexityEntry;

int generate_complexity_report(const char *output_path)
{
    if (output_path == NULL)
    {
        printf("Invalid complexity report path.\n");
        return 1;
    }

    const ComplexityEntry entries[] = {
        {
            "Merge Sort",
            "Divide and Conquer",
            "O(n log n)",
            "O(n log n)",
            "O(n log n)",
            "O(n)",
            "Polynomial"
        },
        {
            "Quick Sort",
            "Divide and Conquer",
            "O(n log n)",
            "O(n log n)",
            "O(n^2)",
            "O(log n) average stack",
            "Polynomial"
        },
        {
            "Binary Search",
            "Searching",
            "O(1)",
            "O(log n)",
            "O(log n)",
            "O(1) iterative",
            "Polynomial"
        },
        {
            "Activity Selection",
            "Greedy",
            "O(n log n)",
            "O(n log n)",
            "O(n log n)",
            "Implementation dependent",
            "Polynomial"
        },
        {
            "Huffman Coding",
            "Greedy",
            "O(n log n)",
            "O(n log n)",
            "O(n log n)",
            "O(n)",
            "Polynomial"
        },
        {
            "Dijkstra",
            "Greedy",
            "O(V^2)",
            "O(V^2)",
            "O(V^2)",
            "O(V)",
            "Polynomial"
        },
        {
            "Prim",
            "Greedy",
            "O(V^2)",
            "O(V^2)",
            "O(V^2)",
            "O(V)",
            "Polynomial"
        },
        {
            "Kruskal",
            "Greedy",
            "O(E log E)",
            "O(E log E)",
            "O(E log E)",
            "O(V+E)",
            "Polynomial"
        },
        {
            "0/1 Knapsack DP",
            "Dynamic Programming",
            "O(nW)",
            "O(nW)",
            "O(nW)",
            "O(nW) full table",
            "Pseudo-polynomial"
        },
        {
            "LCS",
            "Dynamic Programming",
            "O(nm)",
            "O(nm)",
            "O(nm)",
            "O(nm)",
            "Polynomial"
        },
        {
            "Matrix Chain Multiplication",
            "Dynamic Programming",
            "O(n^3)",
            "O(n^3)",
            "O(n^3)",
            "O(n^2)",
            "Polynomial"
        },
        {
            "N-Queens",
            "Backtracking",
            "Input dependent",
            "Input dependent",
            "O(n!) upper bound",
            "O(n)",
            "Exponential search"
        },
        {
            "0/1 Knapsack Branch and Bound",
            "Branch and Bound",
            "Input dependent",
            "Input dependent",
            "O(2^n)",
            "O(n)",
            "Exponential worst case"
        },
        {
            "Edmonds-Karp",
            "Network Flow",
            "Input dependent",
            "Input dependent",
            "O(VE^2)",
            "O(V^2) matrix",
            "Polynomial"
        },
        {
            "Bipartite Matching",
            "Augmenting Paths",
            "Input dependent",
            "Input dependent",
            "O(VE)",
            "O(V)",
            "Polynomial"
        },
        {
            "Chinese Remainder Theorem",
            "Number Theory",
            "Depends on bit lengths",
            "Depends on bit lengths",
            "Polynomial bit complexity",
            "O(n) input storage",
            "Polynomial"
        },
        {
            "KMP String Matching",
            "String Matching",
            "O(n+m)",
            "O(n+m)",
            "O(n+m)",
            "O(m)",
            "Polynomial"
        }
    };

    size_t count =
        sizeof(entries) / sizeof(entries[0]);

    FILE *fp = fopen(output_path, "w");

    if (fp == NULL)
    {
        printf("Cannot create complexity report.\n");
        printf("Make sure reports/ exists.\n");
        return 1;
    }

    if (fprintf(
            fp,
            "Algorithm,Paradigm,Best Case,"
            "Average Case,Worst Case,Space,Classification\n"
        ) < 0)
    {
        fclose(fp);
        return 1;
    }

    printf("\n============================================\n");
    printf("       ALGORITHM COMPLEXITY REPORT\n");
    printf("============================================\n");

    for (size_t i = 0; i < count; i++)
    {
        const ComplexityEntry *e = &entries[i];

        if (fprintf(
                fp,
                "\"%s\",\"%s\",\"%s\",\"%s\","
                "\"%s\",\"%s\",\"%s\"\n",
                e->algorithm,
                e->paradigm,
                e->best,
                e->average,
                e->worst,
                e->space,
                e->classification
            ) < 0)
        {
            printf("Failed to write complexity report.\n");
            fclose(fp);
            return 1;
        }

        printf("%2zu. %-32s | %s\n",
               i + 1,
               e->algorithm,
               e->classification);
    }

    if (fclose(fp) != 0)
    {
        printf("Failed to close complexity report.\n");
        return 1;
    }

    printf("\nAlgorithms Classified: %zu\n", count);
    printf("CSV Report: %s\n", output_path);
    printf("============================================\n");

    return 0;
}
