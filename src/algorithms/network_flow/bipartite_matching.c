
#include <stdio.h>
#include "bipartite_matching.h"

/* Find an augmenting path using DFS */
static int find_match(
    int case_index,
    int compatibility[MAX_CASES][MAX_INVESTIGATORS],
    int investigators,
    int visited[],
    int assigned[])
{
    for (int j = 0; j < investigators; j++) {

        if (compatibility[case_index][j] && !visited[j]) {

            visited[j] = 1;

            if (assigned[j] == -1 ||
                find_match(
                    assigned[j],
                    compatibility,
                    investigators,
                    visited,
                    assigned
                )) {

                assigned[j] = case_index;
                return 1;
            }
        }
    }

    return 0;
}

void assign_investigators(
    int compatibility[MAX_CASES][MAX_INVESTIGATORS],
    int cases,
    int investigators,
    const unsigned long long transaction_ids[])
{
    if (compatibility == NULL ||
        transaction_ids == NULL ||
        cases <= 0 ||
        cases > MAX_CASES ||
        investigators <= 0 ||
        investigators > MAX_INVESTIGATORS) {

        printf("Invalid bipartite matching input.\n");
        return;
    }

    int assigned[MAX_INVESTIGATORS];
    int matched_cases = 0;

    for (int j = 0; j < investigators; j++) {
        assigned[j] = -1;
    }

    for (int i = 0; i < cases; i++) {

        int visited[MAX_INVESTIGATORS] = {0};

        if (find_match(
                i,
                compatibility,
                investigators,
                visited,
                assigned)) {

            matched_cases++;
        }
    }

    printf("\n============================================\n");
    printf("       BIPARTITE MATCHING MODULE\n");
    printf("============================================\n");

    printf("Total Suspicious Cases : %d\n", cases);
    printf("Available Investigators: %d\n", investigators);

    printf("\nInvestigation Assignments:\n");

    for (int j = 0; j < investigators; j++) {

        if (assigned[j] != -1) {
            printf("Investigator %d -> Transaction %llu\n",
                   j + 1,
                   transaction_ids[assigned[j]]);
        }
    }

    printf("\nSuccessfully Assigned : %d\n", matched_cases);
    printf("Unassigned Cases      : %d\n",
           cases - matched_cases);

    printf("============================================\n");
}
