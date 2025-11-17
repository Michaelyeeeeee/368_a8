#include "dijkstra.h"

// source is starting node, *graph[] is pointer to adjacency list, size is number of vertices, heap_index[] is position of each vertex
void dijkstra(int source, gnode **graph, short size)
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

    while (n != 0)
    {
        dequeue(arr, n - 1); // downward heapify
        n--;
        int u = arr[n].label;

        if (u == size - 1)
        {
            // recreate path using predecessor
            // return tnode arr[u];
        }

        gnode *v = graph[u];
        while (v != NULL)
        {
            if (heap_index[v->label] < n && arr[heap_index[v->label]].distance > arr[heap_index[u]].distance + v->weight)
            {
                arr[heap_index[v->label]].distance = arr[heap_index[u]].distance + v->weight;
                arr[heap_index[v->label]].predecessor = u;
                update(arr, heap_index[v->label]); // upward heapify
            }
            v = v->next;
        }
    }
}

void dequeue(tnode **arr, short n)
{                         // n is the last index
    tnode *temp = arr[n]; // exchange the root and the last node
    arr[n] = arr[0];      // update heap_index!
    arr[0] = temp;        // update heap_index!
    n--;                  // decrease the heap size
    short i = 0, j;
    while ((j = 2 * i + 1) <= n)
    {                                                         // left child exists?
        if (j < n && arr[j]->distance < arr[j + 1]->distance) // binary search
            j = j + 1;                                        // pick right child
        if (temp->distance >= arr[j]->distance)
            break;
        else
        {
            arr[i] = arr[j]; // update heap_index!
            i = j;
        }
    }
    arr[i] = temp; // update heap_index!
}

void update(tnode **arr, short i)
{
    tnode *temp = arr[i]; // new element at index i
    short j = i;
    while (j > 0 && arr[(j - 1) / 2]->distance < temp->distance)
    {
        arr[j] = arr[(j - 1) / 2]; // update heap_index!
        j = (j - 1) / 2;
    }
    arr[j] = temp; // update heap_index!
}