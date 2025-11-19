#pragma once

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

typedef struct _tnode
{
    short label;
    int distance;
    short predecessor;
} tnode;

// Adjacency list node
typedef struct gnode
{
    short label;
    short weight;
    struct gnode *next;
} gnode;

int bellman_ford(int source, gnode **graph, short size, short cols, short *cell_costs, tnode path[]);

void write_text_file(short m, short n, short *cell_costs, char *filename);
void write_fastest_time(short n, int *best_times, char *filename);
void write_path(tnode *path, int fastest_time, short cols, char *filename);
void free_graph(gnode **adj_list, int size);
int add_edge(gnode **adj_list, int u, int v, short weight);