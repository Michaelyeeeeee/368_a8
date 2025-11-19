#include "bellman.h"

int main(int argc, char **argv)
{
    if (argc != 5)
        return EXIT_FAILURE;

    FILE *input = fopen(argv[1], "rb");
    if (!input)
        return EXIT_FAILURE;

    short m, n;
    fread(&m, sizeof(short), 1, input);
    fread(&n, sizeof(short), 1, input);
    int total_nodes = m * n;

    // Allocations
    short *cell_costs = malloc(total_nodes * sizeof(short)); // weight of all nodes in an array
    gnode **adj_list = calloc(total_nodes, sizeof(gnode *)); // adjacency list
    int *fastest_times = malloc(n * sizeof(int));
    tnode *current_path = malloc((total_nodes + 1) * sizeof(tnode));
    tnode *best_path = malloc((total_nodes + 1) * sizeof(tnode));

    if (!cell_costs || !adj_list ||
        !fastest_times || !current_path || !best_path)
    {
        return EXIT_FAILURE;
    }

    for (int i = 0; i < total_nodes; i++)
    {
        short cost;
        fread(&cost, sizeof(short), 1, input);
        cell_costs[i] = cost;

        // prints for test
        // printf("cell_cost: %hd\n", cost);
    }
    fclose(input);

    write_text_file(m, n, cell_costs, argv[2]);

    // build adjac list
    // Up, Down, Left, Right
    // used for determining position of next node
    int DIR_ROW[] = {-1, 1, 0, 0};
    int DIR_COL[] = {0, 0, -1, 1};

    for (int row = 0; row < m; row++)
    {
        for (int col = 0; col < n; col++)
        {
            int current_idx = row * n + col;
            short weight = cell_costs[current_idx];

            // Check all 4 neighbors
            for (int dir = 0; dir < 4; dir++)
            {
                int next_row = row + DIR_ROW[dir];
                int next_col = col + DIR_COL[dir];

                // if neighbor is within grid bounds, add edge to graph
                if (next_row >= 0 && next_row < m &&
                    next_col >= 0 && next_col < n)
                {
                    int neighbor_idx = next_row * n + next_col;
                    add_edge(adj_list, current_idx, neighbor_idx, weight);
                }
            }
        }
    }

    int global_min_time = INT_MAX;
    best_path[0].label = -1;

    // Run Bellman-Ford for each starting column
    for (short start_col = 0; start_col < n; start_col++)
    {
        int source = start_col;
        int time = bellman_ford(source, adj_list, (short)total_nodes, n, cell_costs, current_path);

        // add fastest time of each node to an array
        fastest_times[start_col] = time;

        // prints fastest time for col
        // printf("fastest_times: %d\n", time);

        // finds whether current path is fastest
        if (time < global_min_time)
        {
            global_min_time = time;
            int idx = 0;
            while (1)
            {
                best_path[idx] = current_path[idx];
                if (current_path[idx].label == -1)
                    break;
                idx++;
                if (idx > total_nodes)
                    break;
            }
        }
    }

    // print min path
    // printf("minium length: %d\n", global_min_time);

    // print path
    /*
    printf("Path:\n");
    int p_idx = 0;
    while (best_path[p_idx].label != -1 && p_idx < total_nodes)
    {
        printf("path step %d: node %hd\n", p_idx, best_path[p_idx].label);
        p_idx++;
    } */
    write_fastest_time(n, fastest_times, argv[3]);
    write_path(best_path, global_min_time, n, argv[4]);

    free(cell_costs);
    free_graph(adj_list, total_nodes);
    free(fastest_times);
    free(current_path);
    free(best_path);

    return EXIT_SUCCESS;
}