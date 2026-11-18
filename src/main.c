
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "transaction.h"
#include "dataset.h"
#include "fraud.h"
#include "sorting.h"
#include "searching.h"
#include "performance.h"
#include "activity_selection.h"
#include "huffman.h"
#include "graph.h"
#include "mst.h"
#include "knapsack.h"
#include "lcs.h"
#include "mcm.h"
#include "nqueens.h"
#include "branch_bound.h"
#include "max_flow.h"
#include "bipartite_matching.h"
#include "crt.h"
#include "kmp.h"
#include "test_runner.h"
#include "benchmark.h"
#include "stress_test.h"
#include "comparison.h"
#include "complexity_report.h"
#include "scalability.h"
#include "fraud_evaluation.h"



/* Check whether transactions are sorted */
int is_sorted(Transaction *arr, size_t n)
{
    for (size_t i = 1; i < n; i++)
    {
        if (arr[i - 1].transaction_id >
            arr[i].transaction_id)
        {
            return 0;
        }
    }

    return 1;
}

int main(int argc, char *argv[])
{
    /* Run automated tests only */
    if (argc > 1 && strcmp(argv[1], "--test") == 0)
    {
        return run_all_tests();
    }

    /* Run sorting performance benchmarks only */
if (argc > 1 && strcmp(argv[1], "--benchmark") == 0)
{
    return run_sorting_benchmarks(
        "reports/sorting_benchmark.csv"
    );
}

/* Stress testing mode */
if (argc > 1 && strcmp(argv[1], "--stress") == 0)
{
    return run_stress_tests(
        "reports/stress_test_report.csv"
    );
}

if (argc > 1 && strcmp(argv[1], "--compare") == 0)
{
    return run_algorithm_comparison(
        "reports/algorithm_comparison.csv"
    );
}

if (argc > 1 && strcmp(argv[1], "--complexity") == 0)
{
    return generate_complexity_report(
        "reports/complexity_report.csv"
    );
}

if (argc > 1 && strcmp(argv[1], "--scalability") == 0)
{
    return run_dataset_scalability(
        "reports/dataset_scalability.csv"
    );
}

if (argc > 1 && strcmp(argv[1], "--evaluate") == 0)
{
    return run_fraud_evaluation(
        "reports/fraud_evaluation.csv"
    );
}

    Dataset dataset;

    dataset.items = NULL;
    dataset.count = 0;

    printf("\n");
    printf("============================================\n");
    printf("       BANKING FRAUD INVESTIGATION SYSTEM\n");
    printf("============================================\n");

    /* 1. LOAD DATASET */

    printf("\nLoading transaction dataset...\n");

    if (!load_dataset(
            "data/small/transactions_small.csv",
            &dataset))
    {
        printf("Dataset loading failed.\n");
        return 1;
    }

    printf("\nDataset loaded successfully!\n");
    printf("Total Transactions: %zu\n", dataset.count);

    if (dataset.count == 0)
    {
        printf("No transactions available.\n");
        free_dataset(&dataset);
        return 1;
    }

    /* 2. SORTING AND SEARCHING */

    printf("\n");
    printf("============================================\n");
    printf("       SORTING AND SEARCHING MODULE\n");
    printf("============================================\n");

    unsigned long long target_id =
        dataset.items[0].transaction_id;

    Transaction *quick_items = malloc(
        dataset.count * sizeof(Transaction)
    );

    if (quick_items == NULL)
    {
        printf("Memory allocation failed.\n");
        free_dataset(&dataset);
        return 1;
    }

    memcpy(
        quick_items,
        dataset.items,
        dataset.count * sizeof(Transaction)
    );

    /* 3. MERGE SORT */

    printf("\nRunning Merge Sort...\n");

    if (!merge_sort_transactions(
            dataset.items,
            dataset.count))
    {
        printf("Merge Sort failed.\n");

        free(quick_items);
        free_dataset(&dataset);
        return 1;
    }

    if (is_sorted(dataset.items, dataset.count))
    {
        printf("Merge Sort: PASSED\n");
    }
    else
    {
        printf("Merge Sort: FAILED\n");
    }

    /* 4. QUICK SORT */

    printf("\nRunning Quick Sort...\n");

    quick_sort_transactions(
        quick_items,
        dataset.count
    );

    if (is_sorted(quick_items, dataset.count))
    {
        printf("Quick Sort: PASSED\n");
    }
    else
    {
        printf("Quick Sort: FAILED\n");
    }

    /* 5. BINARY SEARCH */

    printf("\nRunning Binary Search...\n");

    ptrdiff_t result = binary_search_transaction(
        dataset.items,
        dataset.count,
        target_id
    );

    if (result != -1)
    {
        printf("Transaction Found!\n");
        printf("Transaction ID: %llu\n", target_id);
        printf("Array Index: %td\n", result);
    }
    else
    {
        printf("Transaction not found.\n");
    }

    printf("\nAll sorting and searching tests completed.\n");

    free(quick_items);

    /* Performance comparison */
    compare_sorting_algorithms(
        dataset.items,
        dataset.count
    );

    /* 6. FRAUD DETECTION */

    printf("\n");
    printf("============================================\n");
    printf("             FRAUD DETECTION\n");
    printf("============================================\n");

    int suspicious = 0;

    for (size_t i = 0; i < dataset.count; i++)
    {
        evaluate_fraud(&dataset.items[i]);

        if (dataset.items[i].risk_score >= 25)
        {
            suspicious++;

            printf("\n[SUSPICIOUS TRANSACTION]\n");
            display_transaction(dataset.items[i]);
        }
    }

    /* 7. INVESTIGATION SUMMARY */

    printf("\n");
    printf("============================================\n");
    printf("          INVESTIGATION SUMMARY\n");
    printf("============================================\n");

    printf("Total Transactions : %zu\n", dataset.count);
    printf("Suspicious Records : %d\n", suspicious);

    printf("Normal Records     : %zu\n",
           dataset.count - (size_t)suspicious);

    double fraud_percentage =
        (double)suspicious / dataset.count * 100.0;

    printf("Suspicious Rate    : %.2f%%\n",
           fraud_percentage);

    printf("============================================\n");
    printf("\nInvestigation completed successfully!\n");

    /* 8. GREEDY - ACTIVITY SELECTION */

    Investigation investigations[] = {
        {1003, 1, 3},
        {1004, 2, 5},
        {1006, 4, 6},
        {1008, 6, 8},
        {1010, 7, 9}
    };

    size_t investigation_count =
        sizeof(investigations) /
        sizeof(investigations[0]);

    schedule_investigations(
        investigations,
        investigation_count
    );

    /* 9. HUFFMAN CODING */

    huffman_compress("FRAUD DETECTION SYSTEM");

    /* 10. GRAPH ALGORITHMS */

    int graph[MAX_NODES][MAX_NODES] = {0};
    int n = 4;

    graph[0][1] = 4;
    graph[0][2] = 2;

    graph[1][0] = 4;
    graph[1][2] = 1;
    graph[1][3] = 5;

    graph[2][0] = 2;
    graph[2][1] = 1;
    graph[2][3] = 8;

    graph[3][1] = 5;
    graph[3][2] = 8;

    dijkstra(graph, n, 0);

    /* Minimum Spanning Tree */
    prim_mst(graph, n);
    kruskal_mst(graph, n);

    /* 11. DYNAMIC PROGRAMMING - KNAPSACK */

    InvestigationItem investigation_items[] = {
        {1003, 2, 30},
        {1004, 3, 40},
        {1006, 4, 50},
        {1008, 5, 60},
        {1010, 1, 20}
    };

    size_t item_count =
        sizeof(investigation_items) /
        sizeof(investigation_items[0]);

    int investigation_budget = 8;

    knapsack_investigations(
        investigation_items,
        item_count,
        investigation_budget
    );

    /* 12. BRANCH AND BOUND - KNAPSACK */

    branch_bound_knapsack(
        investigation_items,
        item_count,
        investigation_budget
    );

    /* 13. LONGEST COMMON SUBSEQUENCE */

    const char *transaction_pattern1 = "ABCDEF";
    const char *transaction_pattern2 = "ACE";

    compare_transaction_patterns(
        transaction_pattern1,
        transaction_pattern2
    );

    /* 14. MATRIX CHAIN MULTIPLICATION */

    int matrix_dimensions[] = {10, 30, 5, 60};

    size_t matrix_count =
        sizeof(matrix_dimensions) /
        sizeof(matrix_dimensions[0]) - 1;

    matrix_chain_multiplication(
        matrix_dimensions,
        matrix_count
    );

    /* 15. BACKTRACKING - N QUEENS */

    solve_nqueens(4);

    /* 16. MAXIMUM FLOW - EDMONDS KARP */

    int capacity[MAX_NODES][MAX_NODES] = {0};

    capacity[0][1] = 10;
    capacity[0][2] = 5;

    capacity[1][2] = 3;
    capacity[1][3] = 7;

    capacity[2][3] = 8;

    edmonds_karp(capacity, 4, 0, 3);

    /* 17. BIPARTITE MATCHING */

    unsigned long long case_ids[] = {
        1003, 1004, 1006, 1008, 1010
    };

    int compatibility[MAX_CASES][MAX_INVESTIGATORS] = {0};

    compatibility[0][0] = 1;
    compatibility[0][1] = 1;

    compatibility[1][0] = 1;

    compatibility[2][1] = 1;
    compatibility[2][2] = 1;

    compatibility[3][2] = 1;

    compatibility[4][0] = 1;
    compatibility[4][2] = 1;

    assign_investigators(
        compatibility,
        5,
        3,
        case_ids
    );

    /* 18. CHINESE REMAINDER THEOREM */

    long long remainders[] = {2, 3, 2};
    long long moduli[] = {3, 5, 7};

    size_t congruence_count =
        sizeof(remainders) /
        sizeof(remainders[0]);

    solve_crt(
        remainders,
        moduli,
        congruence_count
    );

    /* 19. KMP STRING MATCHING */

    printf("\n============================================\n");
    printf("          KMP STRING MATCHING\n");
    printf("============================================\n");

    const char *transaction_text =
        "PAYMENT FROM UNKNOWN DEVICE DETECTED";

    kmp_search(
        transaction_text,
        "UNKNOWN DEVICE"
    );

    analyze_transaction_description(
        1003,
        "UNAUTHORIZED PAYMENT FROM UNKNOWN DEVICE"
    );

    analyze_transaction_description(
        1006,
        "FAILED LOGIN WITH MULTIPLE ATTEMPTS"
    );

    analyze_transaction_description(
        1008,
        "REGULAR PAYMENT COMPLETED SUCCESSFULLY"
    );

    /* 20. AUTOMATED ALGORITHM TESTING */

    int test_status = run_all_tests();

    if (test_status == 0)
    {
        printf("\nAll automated tests passed!\n");
    }
    else
    {
        printf("\nSome automated tests failed.\n");
    }

    /* 21. FREE MEMORY */

    free_dataset(&dataset);

    return test_status;
}
