#include "a8.h"
#include <string.h>

// Helper function to add a directed edge to the adjacency list
// The weight of the edge u->v is the cost of cell u (pay-on-depart)
int add_edge(gnode **adj_list, int u, int v, short weight)
{
    gnode *new_node = malloc(sizeof(gnode));
    if (!new_node)
        return 0; // Allocation failure

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

int main(int argc, char **argv)
{
    // 1. Argument Validation
    if (argc != 5)
    {
        return EXIT_FAILURE;
    }

    // 2. Open Input File
    FILE *input = fopen(argv[1], "rb");
    if (!input)
    {
        return EXIT_FAILURE;
    }

    // 3. Read Dimensions
    short m, n;
    if (fread(&m, sizeof(short), 1, input) != 1 ||
        fread(&n, sizeof(short), 1, input) != 1)
    {
        fclose(input);
        return EXIT_FAILURE;
    }

    int total_nodes = m * n;

    // 4. Memory Allocation
    // We allocate cell_costs and the adjacency list.
    // We also allocate a 2D-accessible structure for write_text_file.

    short *cell_costs = malloc(total_nodes * sizeof(short));
    short **text_grid_rows = malloc(m * sizeof(short *));
    short *text_grid_data = malloc(total_nodes * sizeof(short)); // Contiguous block for text grid
    gnode **adj_list = calloc(total_nodes, sizeof(gnode *));     // Init to NULL

    // Arrays for Dijkstra
    int *fastest_times = malloc(n * sizeof(int));
    tnode *current_path = malloc((total_nodes + 1) * sizeof(tnode)); // +1 for sentinel
    tnode *best_path = malloc((total_nodes + 1) * sizeof(tnode));

    // Check for allocation failures
    if (!cell_costs || !text_grid_rows || !text_grid_data || !adj_list ||
        !fastest_times || !current_path || !best_path)
    {
        // Clean up whatever was allocated
        if (cell_costs)
            free(cell_costs);
        if (text_grid_rows)
            free(text_grid_rows);
        if (text_grid_data)
            free(text_grid_data);
        if (adj_list)
            free_graph(adj_list, total_nodes); // Handles NULL check internally or requires check before
        if (fastest_times)
            free(fastest_times);
        if (current_path)
            free(current_path);
        if (best_path)
            free(best_path);
        fclose(input);
        return EXIT_FAILURE;
    }

    // Setup the 2D pointers for write_text_file
    for (int i = 0; i < m; i++)
    {
        text_grid_rows[i] = text_grid_data + (i * n);
    }

    // 5. Read Grid Data and Populate Structures
    for (int i = 0; i < total_nodes; i++)
    {
        short cost;
        if (fread(&cost, sizeof(short), 1, input) != 1)
        {
            // Handle read error if strictly necessary, otherwise proceed
        }
        cell_costs[i] = cost;
        text_grid_data[i] = cost;
    }
    fclose(input);

    // 6. Build the Graph
    // Iterate through every cell (r, c) and connect to neighbors
    int alloc_success = 1;
    for (int r = 0; r < m; r++)
    {
        for (int c = 0; c < n; c++)
        {
            int u = r * n + c;
            short weight = cell_costs[u]; // Weight is the cost of the CURRENT cell

            // Neighbors: Up, Down, Left, Right
            int dr[] = {-1, 1, 0, 0};
            int dc[] = {0, 0, -1, 1};

            for (int k = 0; k < 4; k++)
            {
                int nr = r + dr[k];
                int nc = c + dc[k];

                if (nr >= 0 && nr < m && nc >= 0 && nc < n)
                {
                    int v = nr * n + nc;
                    if (!add_edge(adj_list, u, v, weight))
                        alloc_success = 0;
                }
            }
        }
    }

    if (!alloc_success)
    {
        // Clean up and exit
        free(cell_costs);
        free(text_grid_rows);
        free(text_grid_data);
        free_graph(adj_list, total_nodes);
        free(fastest_times);
        free(current_path);
        free(best_path);
        return EXIT_FAILURE;
    }

    // 7. Create Output 1: Text Grid
    write_text_file(m, n, text_grid_rows, argv[2]);

    // 8. Run Dijkstra for each start column (0 to n-1)
    int global_min_time = INT_MAX;

    // Initialize best_path with a dummy sentinel in case no path is found (unlikely given problem constraints)
    best_path[0].label = -1;

    for (short start_col = 0; start_col < n; start_col++)
    {
        // Source is always in row 0
        int source_node = start_col; // (0 * n) + start_col

        int time = dijkstra(source_node, adj_list, (short)total_nodes, n, cell_costs, current_path);

        fastest_times[start_col] = time;

        // Check if this is the best overall path
        if (time < global_min_time)
        {
            global_min_time = time;

            // Copy current path to best_path
            // We need to copy until we hit the -1 sentinel in current_path
            int idx = 0;
            while (1)
            {
                best_path[idx] = current_path[idx];
                if (current_path[idx].label == -1)
                    break;
                idx++;
                // Safety break to prevent buffer overflow if logic fails
                if (idx > total_nodes)
                    break;
            }
        }
    }

    // 9. Create Output 2: Fastest Times
    write_fastest_time(n, fastest_times, argv[3]);

    // 10. Create Output 3: Fastest Path
    // Note: if global_min_time is still INT_MAX, it means no path was found.
    // The problem implies valid paths exist.
    write_path(best_path, global_min_time, n, argv[4]);

    // 11. Cleanup and Exit
    free(cell_costs);
    free(text_grid_rows); // Free the array of pointers
    free(text_grid_data); // Free the actual data block
    free_graph(adj_list, total_nodes);
    free(fastest_times);
    free(current_path);
    free(best_path);

    return EXIT_SUCCESS;
}