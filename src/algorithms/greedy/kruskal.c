
#include <stdio.h>
#include <stdlib.h>
#include "mst.h"

typedef struct {
    int u;
    int v;
    int weight;
} Edge;

static int parent[MAX_NODES];
static int rank_set[MAX_NODES];

static int find_set(int x)
{
    if (parent[x] != x) {
        parent[x] = find_set(parent[x]);
    }

    return parent[x];
}

static int union_set(int a, int b)
{
    int root_a = find_set(a);
    int root_b = find_set(b);

    if (root_a == root_b) {
        return 0;
    }

    if (rank_set[root_a] < rank_set[root_b]) {
        parent[root_a] = root_b;
    } else if (rank_set[root_a] > rank_set[root_b]) {
        parent[root_b] = root_a;
    } else {
        parent[root_b] = root_a;
        rank_set[root_a]++;
    }

    return 1;
}

static int compare_edges(const void *a, const void *b)
{
    const Edge *x = (const Edge *)a;
    const Edge *y = (const Edge *)b;

    if (x->weight < y->weight) return -1;
    if (x->weight > y->weight) return 1;
    return 0;
}

void kruskal_mst(int graph[MAX_NODES][MAX_NODES], int n)
{
    if (n <= 0 || n > MAX_NODES) {
        printf("Invalid number of nodes.\n");
        return;
    }

    Edge edges[MAX_NODES * (MAX_NODES - 1) / 2];
    int edge_count = 0;

    for (int i = 0; i < n; i++) {
        parent[i] = i;
        rank_set[i] = 0;

        for (int j = i + 1; j < n; j++) {
            if (graph[i][j] > 0 &&
                graph[i][j] < INF) {

                edges[edge_count].u = i;
                edges[edge_count].v = j;
                edges[edge_count].weight = graph[i][j];
                edge_count++;
            }
        }
    }

    qsort(edges, edge_count, sizeof(Edge), compare_edges);

    int total_cost = 0;
    int selected_edges = 0;

    printf("\n============================================\n");
    printf("           KRUSKAL'S MST ALGORITHM\n");
    printf("============================================\n");

    printf("Selected Edges:\n");

    for (int i = 0; i < edge_count; i++) {
        if (union_set(edges[i].u, edges[i].v)) {
            printf("Account %d -> Account %d | Cost: %d\n",
                   edges[i].u + 1,
                   edges[i].v + 1,
                   edges[i].weight);

            total_cost += edges[i].weight;
            selected_edges++;

            if (selected_edges == n - 1) {
                break;
            }
        }
    }

    if (selected_edges != n - 1) {
        printf("\nGraph is disconnected. MST not possible.\n");
    } else {
        printf("\nTotal MST Cost: %d\n", total_cost);
    }

    printf("============================================\n");
}
