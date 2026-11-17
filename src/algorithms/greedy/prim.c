
#include <stdio.h>
#include "mst.h"

void prim_mst(int graph[MAX_NODES][MAX_NODES], int n)
{
    if (n <= 0 || n > MAX_NODES) {
        printf("Invalid number of nodes.\n");
        return;
    }

    int selected[MAX_NODES] = {0};
    int key[MAX_NODES];
    int parent[MAX_NODES];

    for (int i = 0; i < n; i++) {
        key[i] = INF;
        parent[i] = -1;
    }

    key[0] = 0;

    for (int count = 0; count < n; count++) {
        int min = INF;
        int u = -1;

        for (int i = 0; i < n; i++) {
            if (!selected[i] && key[i] < min) {
                min = key[i];
                u = i;
            }
        }

        if (u == -1) {
            printf("Graph is disconnected. MST not possible.\n");
            return;
        }

        selected[u] = 1;

        for (int v = 0; v < n; v++) {
            int weight = graph[u][v];

            if (!selected[v] &&
                weight > 0 &&
                weight < key[v]) {

                key[v] = weight;
                parent[v] = u;
            }
        }
    }

    int total_cost = 0;

    printf("\n============================================\n");
    printf("            PRIM'S MST ALGORITHM\n");
    printf("============================================\n");

    printf("Selected Edges:\n");

    for (int i = 1; i < n; i++) {
        printf("Account %d -> Account %d | Cost: %d\n",
               parent[i] + 1, i + 1, key[i]);

        total_cost += key[i];
    }

    printf("\nTotal MST Cost: %d\n", total_cost);
    printf("============================================\n");
}
