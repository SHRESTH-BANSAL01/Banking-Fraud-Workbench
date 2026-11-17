
#include <stdio.h>
#include "graph.h"

void dijkstra(int graph[MAX_NODES][MAX_NODES],
              int n, int source)
{
    if (n <= 0 || n > MAX_NODES ||
        source < 0 || source >= n) {
        printf("Invalid graph or source node.\n");
        return;
    }

    int distance[MAX_NODES];
    int visited[MAX_NODES] = {0};
    int parent[MAX_NODES];

    for (int i = 0; i < n; i++) {
        distance[i] = INF;
        parent[i] = -1;
    }

    distance[source] = 0;

    for (int count = 0; count < n; count++) {
        int min_distance = INF;
        int u = -1;

        for (int i = 0; i < n; i++) {
            if (!visited[i] && distance[i] < min_distance) {
                min_distance = distance[i];
                u = i;
            }
        }

        if (u == -1) {
            break;
        }

        visited[u] = 1;

        for (int v = 0; v < n; v++) {
            int weight = graph[u][v];

            if (!visited[v] &&
                weight > 0 &&
                weight < INF &&
                distance[u] <= INF - weight &&
                distance[u] + weight < distance[v]) {

                distance[v] = distance[u] + weight;
                parent[v] = u;
            }
        }
    }

    printf("\n============================================\n");
    printf("         DIJKSTRA SHORTEST PATH\n");
    printf("============================================\n");

    printf("Source Account: %d\n\n", source + 1);

    for (int i = 0; i < n; i++) {
        printf("Account %d -> Account %d : ",
               source + 1, i + 1);

        if (distance[i] == INF) {
            printf("No Path\n");
        } else {
            printf("Cost = %d | Path: ", distance[i]);

            int path[MAX_NODES];
            int length = 0;
            int current = i;

            while (current != -1 && length < MAX_NODES) {
                path[length++] = current;
                current = parent[current];
            }

            for (int j = length - 1; j >= 0; j--) {
                printf("%d", path[j] + 1);

                if (j > 0) {
                    printf(" -> ");
                }
            }

            printf("\n");
        }
    }

    printf("============================================\n");
}
