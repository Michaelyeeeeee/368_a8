#include "bellman.h"

/// bellman ford to update all nodes every iteration
// source is starting node
// graph is graph from reading file
// size is total number of vertices/ points on graph
// cols is number of cols read from file
// cell costs is the weight of each node where the weight of node (a,b) is cell_costs[a * cols + b]
// path_output is shortest path
int bellman_ford(int source, gnode **graph, short size, short cols, short *cell_costs, tnode path_output[])
{
    tnode *state = malloc(size * sizeof(tnode)); // min heap
    if (!state)
        return -1; // Error

    // 1. Init
    for (int i = 0; i < size; i++)
    {
        state[i].label = i;
        state[i].distance = INT_MAX;
        state[i].predecessor = -1;
    }
    state[source].distance = 0;

    // update until nothing changes
    for (int i = 1; i < size; i++)
    {
        int changed = 0;
        for (int u = 0; u < size; u++)
        {
            if (state[u].distance == INT_MAX)
                continue;
            gnode *adj = graph[u];
            while (adj != NULL)
            {
                int v = adj->label;
                if (state[u].distance + adj->weight < state[v].distance)
                {
                    state[v].distance = state[u].distance + adj->weight;
                    state[v].predecessor = u;
                    changed = 1;
                }
                adj = adj->next;
            }
        }
        if (!changed)
            break;
    }

    // look at times of bottom row
    int best_exit_time = INT_MAX;
    int best_node;
    // start of bottom = size - cols
    for (int i = size - cols; i < size; i++)
    {
        if (state[i].distance != INT_MAX)
        {
            // adds current cell cost to distance for actual times
            int total = state[i].distance + cell_costs[i];
            // updates best time and best starting node
            if (total < best_exit_time)
            {
                best_exit_time = total;
                best_node = i;
            }
        }
    }

    // makes path
    int curr = best_node;
    // counts number of nodes in path (ends at -1)
    int count = 0;
    int temp = curr;
    while (temp != -1)
    {
        count++;
        if (temp == source)
            break;
        temp = state[temp].predecessor;
    }

    // Write in reverse order b/c path[0] is last node
    curr = best_node;
    int idx = count - 1;
    while (curr != -1 && idx >= 0)
    {
        path_output[idx].label = state[curr].label;
        path_output[idx].distance = state[curr].distance;
        path_output[idx].predecessor = state[curr].predecessor;
        curr = state[curr].predecessor;
        idx--;
    }
    // -1 to mark end of path
    if (count < size)
        path_output[count].label = -1;

    free(state);
    return best_exit_time;
}

// writes graph to text file
void write_text_file(short m, short n, short *cell_costs, char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
        return;

    fprintf(file, "%hd %hd\n", m, n);

    for (int r = 0; r < m; r++)
    {
        for (int c = 0; c < n; c++)
        {
            // Calculate 1D index from 2D coordinates
            int index = r * n + c;

            // Use the index to access the flat array
            fprintf(file, "%hd%s", cell_costs[index], (c == n - 1) ? "" : " ");
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

// writes columns and best times for each starting node
void write_fastest_time(short n, int *best_times, char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
        return;
    fwrite(&n, sizeof(short), 1, file);
    fwrite(best_times, sizeof(int), n, file);
    fclose(file);
}

// writes fastest overall time and shortest pathh
void write_path(tnode *path, int fastest_time, short cols, char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
        return;
    fwrite(&fastest_time, sizeof(int), 1, file);

    int l = 0;
    while (path[l].label != -1)
        l++;
    fwrite(&l, sizeof(int), 1, file);

    for (int i = 0; i < l; i++)
    {
        short r = path[i].label / cols;
        short c = path[i].label % cols;
        fwrite(&r, sizeof(short), 1, file);
        fwrite(&c, sizeof(short), 1, file);
    }
    fclose(file);
}

// creates edges for node in cell_costs from cell_costs array
int add_edge(gnode **adj_list, int u, int v, short weight)
{
    gnode *new_node = malloc(sizeof(gnode));
    if (!new_node)
        return 0;
    new_node->label = (short)v;
    new_node->weight = weight;
    new_node->next = adj_list[u];
    adj_list[u] = new_node;
    return 1;
}

void free_graph(gnode **adj_list, int size)
{
    for (int i = 0; i < size; i++)
    {
        gnode *curr = adj_list[i];
        while (curr != NULL)
        {
            gnode *temp = curr;
            curr = curr->next;
            free(temp);
        }
    }
    free(adj_list);
}