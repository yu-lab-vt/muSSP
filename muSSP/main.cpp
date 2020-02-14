#include <iostream>
#include <fstream>
#include <stdio.h>
#include "Graph.h"
#include <ctime>
#include <numeric>
#include <algorithm>

///
/// \brief node_key
/// \param i
/// \param j
/// \return
///
inline size_t node_key(int i, int j)
{
    return (size_t) i << 32 | (unsigned int) j;
}

///
/// \brief init
/// \param filename
/// \return
///
Graph init(std::string filename)
{
    char pr_type[3]; ////problem type;

    std::vector<int> edge_tails, edge_heads;
    std::vector<double> edge_weights;

    std::ifstream file(filename);

    int n = 0, m = 0; ////no of nodes, no of arcs;
    std::string line_inf;
    getline(file, line_inf);
    //cout << line << std::endl;
    sscanf(line_inf.c_str(), "%*c %3s %d %d", pr_type, &n, &m);

    double en_weight = 0;
    double ex_weight = 0;
    //getline(file, line_inf);
    //cout << line << std::endl;
    //sscanf(line_inf.c_str(), "%*c %4s %lf %lf", pr_type, &en_weight, &ex_weight);

    auto *resG = new Graph(n, m, 0, n - 1, en_weight, ex_weight);
    int edges = 0;
    int edge_id = 0;
    std::cout << "Parsing edges: " << std::endl;
    for (std::string line; getline(file, line);) {
        switch (line[0]) {
            case 'c':                  /* skip lines with comments */
            case '\n':                 /* skip empty lines   */
            case 'n':
            case '\0':                 /* skip empty lines at the end of file */
                break;
            case 'p':
            case 'a':
        {
            int tail = 0;
            int head = 0;
            double weight = 0;
                sscanf(line.c_str(), "%*c %d %d %lf", &tail, &head, &weight);
                edges++;

                resG->add_edge(tail - 1, head - 1, edge_id, weight);
                edge_id++;
                if (edges % 10000 == 0)
                    std::cout << edges << std::endl;
                break;
            }
            default:
                break;
        }
    }
    std::cout <<"Parsing done!"<< std::endl;
    return *resG;
}

///
/// \brief print_solution
///        output the min-cost flow results
/// \param resG
/// \param path_set
/// \param outfile_name
///
void print_solution(Graph resG, std::vector<std::vector<int>> path_set, const char *outfile_name)
{
    std::vector<bool> edge_visited_flag(resG.num_edges_);
    for (size_t i = 0; i < path_set.size(); i++) {
        for (size_t j = 0; j < path_set[i].size() - 1; j++) {
            int tail = path_set[i][j];
            int head = path_set[i][j + 1];
            int edge_idx = resG.node_id2edge_id[node_key(tail, head)];
            edge_visited_flag[edge_idx] = !edge_visited_flag[edge_idx];
        }
    }
    FILE *fp = fopen(outfile_name, "w");
    for (int i = 0; i < resG.num_edges_; i++) {
        if (edge_visited_flag[i])
            fprintf(fp, "f %d %d 1\n", resG.edge_tail_head[i].first + 1, resG.edge_tail_head[i].second + 1);
        else
            fprintf(fp, "f %d %d 0\n", resG.edge_tail_head[i].first + 1, resG.edge_tail_head[i].second + 1);
    }
    fclose(fp);
}

///
/// \brief main
/// \param argc
/// \param argv
/// \return
///
int main(int argc, char *argv[])
{
    clock_t t_start;
    clock_t t_end;

    //// reading data
    t_start = clock();
    //Graph org_graph = init("input.txt");
    //Graph org_graph = init(
    //        "input_MOT_seq07_followme_k2.txt");
    char* in_file =  argv[2];
    Graph org_graph = init(in_file);
    t_end = clock();
    long double parsing_time = t_end - t_start;

    auto *duration = new long double[10];
    for (int i = 0; i < 10; i++)
        duration[i] = 0;
    //// 1: remove dummy edges
    t_start = clock();
    org_graph.invalid_edge_rm();
    t_end = clock();
    duration[0] = t_end - t_start;

    ////save path and path cost
    std::vector<double> path_cost;
    std::vector<std::vector<int>> path_set;
    int path_num = 0;
    t_start = clock();
    //// 2: initialize shortest path tree from the DAG
    org_graph.shortest_path_dag();
    t_end = clock();
    duration[1] = duration[1] + t_end - t_start;

    path_cost.push_back(org_graph.distance2src[org_graph.sink_id_]);
    org_graph.cur_path_max_cost = -org_graph.distance2src[org_graph.sink_id_]; // the largest cost we can accept

    //// 3: convert edge cost (make all weights positive)
    t_start = clock();
    org_graph.update_allgraph_weights();
    t_end = clock();
    duration[2] = duration[2] + t_end - t_start;

    //// 8: extract shortest path
    t_start = clock();
    org_graph.extract_shortest_path();
    t_end = clock();
    duration[7] = duration[7] + t_end - t_start;

    path_set.push_back(org_graph.shortest_path);
    path_num++;

    std::vector<unsigned long> update_node_num;

    //// 4: find nodes for updating based on branch node
    std::vector<int> node_id4updating;
    t_start = clock();
    org_graph.find_node_set4update(node_id4updating);
    t_end = clock();
    duration[3] = duration[3] + t_end - t_start;

    //// 10: rebuild residual graph by flipping paths
    t_start = clock();
    org_graph.flip_path();//also erase the top sinker
    t_end = clock();
    duration[9] = duration[9] + t_end - t_start;
    while (true) {
        //// 6: update shortest path tree based on the selected sub-graph
        t_start = clock();
        org_graph.update_shortest_path_tree_recursive(node_id4updating);
        printf("Iteration #%d, updated node number  %ld \n", path_num, org_graph.upt_node_num);
        t_end = clock();
        duration[5] = duration[5] + t_end - t_start;

        //// 7: update sink node (heap)
        t_start = clock();
        org_graph.update_sink_info(node_id4updating);
        t_end = clock();
        duration[6] = duration[6] + t_end - t_start;

        update_node_num.push_back(node_id4updating.size());

        //// 8: extract shortest path
        t_start = clock();
        org_graph.extract_shortest_path();
        t_end = clock();
        duration[7] = duration[7] + t_end - t_start;

        // test if stop
        double cur_path_cost = path_cost[path_num - 1] + org_graph.distance2src[org_graph.sink_id_];

        if (cur_path_cost > -0.0000001) {
            break;
        }

        path_cost.push_back(cur_path_cost);
        org_graph.cur_path_max_cost = -cur_path_cost;
        path_set.push_back(org_graph.shortest_path);
        path_num++;

        //// 9: update weights
        t_start = clock();
        org_graph.update_subgraph_weights(node_id4updating);
        t_end = clock();
        duration[8] = duration[8] + t_end - t_start;

        //// 4: find nodes for updating
        t_start = clock();
        org_graph.find_node_set4update(node_id4updating);
        t_end = clock();
        duration[3] = duration[3] + t_end - t_start;
        //// 10: rebuild the graph
        t_start = clock();
        org_graph.flip_path();
        t_end = clock();
        duration[9] = duration[9] + t_end - t_start;
    }

    //// out put results and time consuming
    std::cout << "Parsing time is: " << parsing_time / CLOCKS_PER_SEC << " s" << std::endl;

    long double all_cpu_time = 0;
    for (int i = 0; i < 10; i++) {
        auto time_elapsed_ms = 1000.0 * duration[i] / CLOCKS_PER_SEC;
        all_cpu_time += time_elapsed_ms;
        std::cout << "the " << i + 1 << " step used: " << time_elapsed_ms / 1000.0 << " s\n";
    }
    std::cout << "The overall time is " << all_cpu_time / 1000.0 << " s\n\n";

    //// start validation
    if (false) {
        double cost_sum = 0, cost_sum_recalculate = 0;
        for (auto &&i : path_cost) {
            cost_sum += i;
        }
        for (auto &&tmpPath:path_set) {
            double tmp_path_cost = 0;
            for (size_t j = 0; j < tmpPath.size() - 1; j++) {
                int tmp_edge_id = org_graph.node_id2edge_id[node_key(tmpPath[j + 1], tmpPath[j])];
                tmp_path_cost += org_graph.edge_org_weights[tmp_edge_id];
                org_graph.edge_org_weights[tmp_edge_id] *= -1;
            }
            cost_sum_recalculate += tmp_path_cost;
        }
        unsigned long total_upt_node_num = 0;
        for (auto &&i : update_node_num) {
            total_upt_node_num += i;
        }
        printf("The number of paths: %ld, total cost is %.7f, real-cost %.7f, final cost is: %.7f.\n",
               path_cost.size(), cost_sum, cost_sum_recalculate, path_cost[path_cost.size() - 1]);
    }
    double cost_sum = 0;
    for (auto &&i : path_cost) {
        cost_sum += i;
    }
    printf("The number of paths: %ld, total cost is %.7f, final path cost is: %.7f.\n",
           path_cost.size(), cost_sum, path_cost[path_cost.size() - 1]);
    //// output the number nodes updated in each iteration
//    FILE *fp;
//    fp = fopen("upt_size.txt", "w");
//    for (auto&& i:update_node_num){
//        fprintf (fp, "%ld\n", i);
//    }
//    fclose(fp);


    /*********write detailed flow to txt********/
    if (argc > 3) {
        print_solution(org_graph, path_set, "output.txt");//"output_edge_rm.txt"
    }
    return 0;
}
