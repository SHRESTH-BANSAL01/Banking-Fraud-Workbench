
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "transaction.h"
#include "sorting.h"
#include "searching.h"
#include "test_runner.h"

static int tests_passed = 0;
static int tests_failed = 0;

/* Print the result of an individual test */
static void report_test(const char *name, int passed)
{
    if (passed) {
        printf("[PASS] %s\n", name);
        tests_passed++;
    } else {
        printf("[FAIL] %s\n", name);
        tests_failed++;
    }
}

/* Create a sample transaction */
static Transaction create_transaction(
    unsigned long long id,
    double amount)
{
    Transaction t = {0};

    t.transaction_id = id;
    t.account_id = 5001;
    t.device_id = 101;
    t.merchant_id = 201;
    t.amount = amount;

    strcpy(t.timestamp, "2026-11-01T10:00:00");

    t.is_fraud = 0;
    t.risk_score = 0;

    return t;
}

/* Check ascending order by transaction ID */
static int is_sorted(const Transaction arr[], size_t n)
{
    for (size_t i = 1; i < n; i++) {
        if (arr[i - 1].transaction_id >
            arr[i].transaction_id) {
            return 0;
        }
    }

    return 1;
}

/* Test 1: Merge Sort */
static void test_merge_sort(void)
{
    Transaction arr[] = {
        create_transaction(1005, 5000),
        create_transaction(1002, 2000),
        create_transaction(1004, 4000),
        create_transaction(1001, 1000),
        create_transaction(1003, 3000)
    };

    size_t n = sizeof(arr) / sizeof(arr[0]);

    int result = merge_sort_transactions(arr, n);

    report_test(
        "Merge Sort",
        result != 0 && is_sorted(arr, n)
    );
}

/* Test 2: Quick Sort */
static void test_quick_sort(void)
{
    Transaction arr[] = {
        create_transaction(1005, 5000),
        create_transaction(1002, 2000),
        create_transaction(1004, 4000),
        create_transaction(1001, 1000),
        create_transaction(1003, 3000)
    };

    size_t n = sizeof(arr) / sizeof(arr[0]);

    quick_sort_transactions(arr, n);

    report_test("Quick Sort", is_sorted(arr, n));
}

/* Test 3: Binary Search */
static void test_binary_search(void)
{
    Transaction arr[] = {
        create_transaction(1001, 1000),
        create_transaction(1002, 2000),
        create_transaction(1003, 3000),
        create_transaction(1004, 4000),
        create_transaction(1005, 5000)
    };

    size_t n = sizeof(arr) / sizeof(arr[0]);

    ptrdiff_t found =
        binary_search_transaction(arr, n, 1003);

    ptrdiff_t missing =
        binary_search_transaction(arr, n, 9999);

    report_test(
        "Binary Search",
        found == 2 && missing == -1
    );
}

/* Test 4: Both sorting algorithms agree */
static void test_sorting_consistency(void)
{
    Transaction original[] = {
        create_transaction(1009, 9000),
        create_transaction(1001, 1000),
        create_transaction(1007, 7000),
        create_transaction(1003, 3000),
        create_transaction(1005, 5000)
    };

    size_t n = sizeof(original) / sizeof(original[0]);

    Transaction merge_arr[5];
    Transaction quick_arr[5];

    memcpy(merge_arr, original, sizeof(original));
    memcpy(quick_arr, original, sizeof(original));

    int merge_ok = merge_sort_transactions(merge_arr, n);

    quick_sort_transactions(quick_arr, n);

    int equal = merge_ok != 0;

    for (size_t i = 0; i < n && equal; i++) {
        if (merge_arr[i].transaction_id !=
            quick_arr[i].transaction_id) {
            equal = 0;
        }
    }

    report_test(
        "Sorting Consistency",
        equal && is_sorted(merge_arr, n)
              && is_sorted(quick_arr, n)
    );
}

/* Test 5: Empty Dataset */
static void test_empty_dataset(void)
{
    Transaction arr[1] = {{0}};

    quick_sort_transactions(arr, 0);

    ptrdiff_t index =
        binary_search_transaction(arr, 0, 1001);

    report_test(
        "Empty Dataset",
        is_sorted(arr, 0) && index == -1
    );
}

/* Test 6: Single Transaction */
static void test_single_transaction(void)
{
    Transaction arr[] = {
        create_transaction(1001, 5000)
    };

    int merge_ok = merge_sort_transactions(arr, 1);

    quick_sort_transactions(arr, 1);

    ptrdiff_t index =
        binary_search_transaction(arr, 1, 1001);

    report_test(
        "Single Transaction",
        merge_ok != 0 &&
        is_sorted(arr, 1) &&
        index == 0
    );
}

/* Main automated test runner */
int run_all_tests(void)
{
    tests_passed = 0;
    tests_failed = 0;

    printf("\n============================================\n");
    printf("          AUTOMATED ALGORITHM TESTS\n");
    printf("============================================\n");

    test_merge_sort();
    test_quick_sort();
    test_binary_search();
    test_sorting_consistency();
    test_empty_dataset();
    test_single_transaction();

    printf("\n============================================\n");
    printf("              TEST SUMMARY\n");
    printf("============================================\n");

    printf("Total Tests : %d\n",
           tests_passed + tests_failed);

    printf("Passed      : %d\n", tests_passed);
    printf("Failed      : %d\n", tests_failed);

    printf("============================================\n");

    return tests_failed == 0 ? 0 : 1;
}
