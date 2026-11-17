
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "kmp.h"


/* Construct the LPS array */
static void compute_lps(
    const char *pattern,
    size_t m,
    size_t lps[])
{
    size_t len = 0;
    size_t i = 1;

    lps[0] = 0;

    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

/* Return the first matching position, or -1 */
static size_t find_pattern(
    const char *text,
    const char *pattern)
{
    size_t n = strlen(text);
    size_t m = strlen(pattern);

    if (m == 0) {
        return 0;
    }

    if (m > n) {
        return SIZE_MAX;
    }

    if (m > SIZE_MAX / sizeof(size_t)) {
        return SIZE_MAX;
    }

    size_t *lps = malloc(m * sizeof(size_t));

    if (lps == NULL) {
        return SIZE_MAX;
    }

    compute_lps(pattern, m, lps);

    size_t i = 0;
    size_t j = 0;
    size_t result = SIZE_MAX;

    while (i < n) {
        if (text[i] == pattern[j]) {
            i++;
            j++;
        }

        if (j == m) {
            result = i - j;
            break;
        }

        if (i < n && text[i] != pattern[j]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    free(lps);
    return result;
}

void kmp_search(
    const char *text,
    const char *pattern)
{
    if (text == NULL || pattern == NULL ||
        pattern[0] == '\0') {

        printf("Invalid KMP input.\n");
        return;
    }

    size_t position = find_pattern(text, pattern);

    printf("\nPattern: %s\n", pattern);

    if (position != SIZE_MAX) {
        printf("Pattern Found at Index: %zu\n", position);
    } else {
        printf("Pattern Not Found\n");
    }
}

void analyze_transaction_description(
    unsigned long long transaction_id,
    const char *description)
{
    if (description == NULL) {
        printf("Invalid transaction description.\n");
        return;
    }

    const char *suspicious_patterns[] = {
        "UNAUTHORIZED",
        "FAILED LOGIN",
        "UNKNOWN DEVICE",
        "MULTIPLE ATTEMPTS",
        "SUSPICIOUS TRANSFER"
    };

    size_t pattern_count =
        sizeof(suspicious_patterns) /
        sizeof(suspicious_patterns[0]);

    int matches = 0;

    printf("\n============================================\n");
    printf("         KMP FRAUD PATTERN ANALYSIS\n");
    printf("============================================\n");

    printf("Transaction ID: %llu\n", transaction_id);
    printf("Description: %s\n", description);

    for (size_t i = 0; i < pattern_count; i++) {
        size_t position =
            find_pattern(description, suspicious_patterns[i]);

        if (position != SIZE_MAX) {
            printf("\nSuspicious Keyword: %s\n",
                   suspicious_patterns[i]);

            printf("Found at Index: %zu\n", position);
            matches++;
        }
    }

    printf("\nTotal Suspicious Keywords: %d\n", matches);

    if (matches > 0) {
        printf("Status: FLAGGED FOR REVIEW\n");
    } else {
        printf("Status: NO KEYWORD MATCHES\n");
    }

    printf("============================================\n");
}
