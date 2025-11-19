#pragma once

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

typedef struct _tnode
{
    short label;
    int distance;
    short predecessor;
} tnode;

typedef struct _gnode
{
    short label;
    short weight;
    struct gnode *next;
} gnode;

int dijkstra(int source, gnode **graph, short size, short cols, short *cell_costs, tnode path[]);
void dequeue(tnode *arr, short last_index, short *heap_index);
void update(tnode *arr, short index, short *heap_index);
void write_text_file(short m, short n, short **graph, char *filename);
void write_fastest_time(short n, int *best_times, char *filename);
void write_path(tnode *path, int fastest_time, short cols, char *filename);