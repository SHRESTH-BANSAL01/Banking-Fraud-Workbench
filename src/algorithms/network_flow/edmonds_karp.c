
#include <stdio.h>
#include <limits.h>
#include "max_flow.h"

/* BFS to find an augmenting path */
static int bfs(
    int residual[MAX_NODES][MAX_NODES],
    int n,
    int source,
    int sink,
    int parent[])
{
    int visited[MAX_NODES] = {0};
    int queue[MAX_NODES];

    int front = 0;
    int rear = 0;

    for (int i = 0; i < n; i++) {
        parent[i] = -1;
    }

    visited[source] = 1;
    queue[rear++] = source;

    while (front < rear) {
        int u = queue[front++];

        for (int v = 0; v < n; v++) {
            if (!visited[v] && residual[u][v] > 0) {
                visited[v] = 1;
                parent[v] = u;

                if (v == sink) {
                    return 1;
                }

                queue[rear++] = v;
            }
        }
    }

    return 0;
}

void edmonds_karp(
    int capacity[MAX_NODES][MAX_NODES],
    int n,
    int source,
    int sink)
{
    if (n <= 0 || n > MAX_NODES ||
        source < 0 || source >= n ||
        sink < 0 || sink >= n ||
        source == sink) {

        printf("Invalid maximum flow input.\n");
        return;
    }

    int residual[MAX_NODES][MAX_NODES] = {0};
    int parent[MAX_NODES];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (capacity[i][j] < 0) {
                printf("Negative capacities are not allowed.\n");
                return;
            }

            residual[i][j] = capacity[i][j];
        }
    }

    int max_flow = 0;
    int augmentations = 0;

    printf("\n============================================\n");
    printf("        EDMONDS-KARP MAXIMUM FLOW\n");
    printf("============================================\n");

    while (bfs(residual, n, source, sink, parent)) {
        int path_flow = INT_MAX;

        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];

            if (residual[u][v] < path_flow) {
                path_flow = residual[u][v];
            }
        }

        if (path_flow > INT_MAX - max_flow) {
            printf("Maximum flow exceeds integer range.\n");
            return;
        }

        /* Check that reverse residual capacities can grow safely */
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];

            if (residual[v][u] > INT_MAX - path_flow) {
                printf("Residual capacity overflow.\n");
                return;
            }
        }

        printf("Augmenting Path: ");

        int path[MAX_NODES];
        int length = 0;

        for (int v = sink; v != source; v = parent[v]) {
            path[length++] = v;
        }

        path[length++] = source;

        for (int i = length - 1; i >= 0; i--) {
            printf("%d", path[i] + 1);

            if (i > 0) {
                printf(" -> ");
            }
        }

        printf(" | Flow: %d\n", path_flow);

        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];

            residual[u][v] -= path_flow;
            residual[v][u] += path_flow;
        }

        max_flow += path_flow;
        augmentations++;
    }

    printf("\nSource Account: %d\n", source + 1);
    printf("Sink Account: %d\n", sink + 1);

    printf("Maximum Network Flow: %d\n", max_flow);
    printf("Augmenting Paths Found: %d\n", augmentations);

    printf("============================================\n");
}
