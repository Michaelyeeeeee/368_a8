#include "dijkstra.h"

// source is starting node, *graph[] is pointer to adjacency list, size is number of vertices (m * n)
// cols is number of columns, cell costs is weight of each vertex, path holds path from bottom to top
int dijkstra(int source, gnode **graph, short size, short cols, short *cell_costs, tnode path[])
{
    tnode arr[size];        // min-heap
    short heap_index[size]; // array to keep track of vertex positions
    short n = size;
    for (short i = 0; i < size; i++)
    {
        arr[i].label = i;
        arr[i].distance = SHRT_MAX;
        arr[i].predecessor = -1;
        heap_index[i] = i;
    }
    arr[0].label = source;
    arr[source].label = 0;
    arr[0].distance = 0;
    heap_index[0] = source;
    heap_index[source] = 0;

    int best_exit = INT_MAX;
    short bottom_start = size - cols;

    if (size > 0)
        path[0].label = -1;

    while (n != 0)
    {
        dequeue(arr, n - 1, heap_index); // downward heapify
        n--;
        int u = arr[n].label;

        // checks if minimum bottom path
        int arrival = arr[n].distance;

        if (u >= bottom_start && u < size)
        {
            if (arrival != SHRT_MAX)
            {
                int exit_time = arrival + cell_costs[u];
                if (exit_time < best_exit)
                {
                    best_exit = exit_time;

                    // if best time, adds it to path
                    short cur = u;
                    short idx = 0;
                    while (cur != -1)
                    {
                        short pos = heap_index[cur];
                        path[idx].label = arr[pos].label;
                        path[idx].distance = arr[pos].distance;
                        path[idx].predecessor = arr[pos].predecessor;
                        idx++;
                        if (cur == source)
                            break;
                        cur = arr[pos].predecessor;
                    }
                    /* Optional: mark termination if there is space */
                    if (idx < size)
                        path[idx].label = -1;
                }
            }
        }

        gnode *v = graph[u];
        while (v != NULL)
        {
            if (heap_index[v->label] < n && arr[heap_index[v->label]].distance > arr[heap_index[u]].distance + v->weight)
            {
                arr[heap_index[v->label]].distance = arr[heap_index[u]].distance + v->weight;
                arr[heap_index[v->label]].predecessor = u;
                update(arr, heap_index[v->label], heap_index); // upward heapify
            }
            v = v->next;
        }
    }

    return best_exit;
}

void dequeue(tnode *arr, short n, short *heap_index)
{                        // n is the last index
    tnode temp = arr[0]; // exchange the root and the last node
    arr[0] = arr[n];
    arr[n] = temp;
    heap_index[arr[0].label] = 0; // update heap_index!
    heap_index[arr[n].label] = n; // update heap_index!
    n--;                          // decrease the heap size
    short i = 0, j;
    while ((j = 2 * i + 1) <= n)
    {                                                       // left child exists?
        if (j < n && arr[j].distance > arr[j + 1].distance) // binary search
            j = j + 1;                                      // pick right child
        if (temp.distance >= arr[j].distance)
            break;
        else
        {
            arr[i] = arr[j]; // update heap_index!
            i = j;
        }
    }
    arr[i] = temp;

    heap_index[temp.label] = i;
}

void update(tnode *arr, short i, short *heap_index)
{
    tnode temp = arr[i]; // new element at index i
    short j = i;
    while (j > 0 && arr[(j - 1) / 2].distance > temp.distance)
    {
        arr[j] = arr[(j - 1) / 2];
        heap_index[arr[j].label] = j; // update heap_index!
        j = (j - 1) / 2;
    }
    arr[j] = temp;

    heap_index[temp.label] = j; // update heap_index!
}

void write_text_file(short m, short n, short **graph, char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        return;
    }
    fprintf(file, "%hd %hd \n", m, n);
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (j == n - 1)
                fprintf(file, "%hd\n", graph[i][j]);
            else
                fprintf(file, "%hd ", graph[i][j]);
        }
    }
    fclose(file);
}

void write_fastest_time(short n, int *best_times, char *filename)
{
    if (filename == NULL || best_times == NULL)
        return;
    FILE *file = fopen(filename, "wb");
    if (!file)
        return;

    short cols = n;
    if (fwrite(&cols, sizeof(short), 1, file) != 1)
    {
        fclose(file);
        return;
    }
    if (fwrite(best_times, sizeof(int), n, file) != (size_t)n)
    {
        fclose(file);
        return;
    }

    fclose(file);
}

void write_path(tnode *path, int fastest_time, short cols, char *filename)
{
    if (path == NULL || filename == NULL)
        return;

    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        return;
    }

    if (fwrite(&fastest_time, sizeof(int), 1, file) != 1)
    {
        fclose(file);
        return;
    }

    int l = 0;
    while (path[l].label != -1)
        l++;

    if (fwrite(&l, sizeof(int), 1, file) != 1)
    {
        fclose(file);
        return;
    }

    for (int i = l - 1; i >= 0; i--)
    {
        short label = path[i].label;
        short row = label / cols;
        short col = label % cols;
        if (fwrite(&row, sizeof(short), 1, file) != 1 || fwrite(&col, sizeof(short), 1, file) != 1)
        {
            fclose(file);
            return;
        }
    }

    fclose(file);
}