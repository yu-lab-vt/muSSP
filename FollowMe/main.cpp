#include <iostream>
#include <fstream>
#include <stdio.h>
#include "Graph.h"
#include <ctime>
#include <numeric>
#include <algorithm>
#include <memory>
#include <iostream>
#include <string>
#include <array>

///
/// \brief init
/// \param filename
/// \return
///
Graph* init(std::string filename)
{
    int n, m; //no of nodes, no of arcs;
    char pr_type[3]; //problem type;
    int tail, head;
    double weight = 0;
    double en_weight = 0;
    double ex_weight = 0;

    std::vector<int> edge_tails, edge_heads;
    std::vector<double> edge_weights;

    std::ifstream file(filename);

    std::string line_inf;
    getline(file, line_inf);
    //cout << line << std::endl;
    sscanf(line_inf.c_str(), "%*c %3s %d %d", pr_type, &n, &m);

    //getline(file, line_inf);
    //cout << line << std::endl;
    //sscanf(line_inf.c_str(), "%*c %4s %lf %lf", pr_type, &en_weight, &ex_weight);

    auto *resG = new Graph(n, m, 0, n-1, en_weight, ex_weight);
    int edges = 0;
    int edge_id = 0;
    for(std::string line; getline(file, line); )
    {
        switch(line[0]){
            case 'c':                  /* skip lines with comments */
            case '\n':                 /* skip empty lines   */
            case 'n':
            case '\0':                 /* skip empty lines at the end of file */
                break;
            case 'p':
            case 'a': {
                sscanf(line.c_str(), "%*c %d %d %lf", &tail, &head, &weight);
                edges++;

                resG->add_edge(tail-1, head-1, edge_id, weight);
                edge_id++;
                if (edges % 10000 == 0)
                    std::cout << edges << std::endl;
                break;
            }
            default:
                break;
        }
    }

    return resG;
}

///
/// \brief main
/// \param argc
/// \param argv
/// \return
///
int main(int argc, char* argv[])
{
    std::string inFileName = (argc > 1) ? argv[1] : "input_MOT_seq07_followme.txt";

    clock_t t_start = clock();
    std::unique_ptr<Graph> org_graph = std::unique_ptr<Graph>(init(inFileName));
    clock_t t_end = clock();

    long double parsing_time = t_end - t_start;
    // //100002, 199988, 399970, 599926, 799868, 999806
    std::array<long double, 10> duration;
    duration.fill(0);

    std::vector<double> path_cost;
    int path_num = 0;
    t_start = clock();
    //     1st step: initialize shortest path tree from the DAG
    org_graph->shortest_path_dag();
    t_end = clock();
    duration[0] += t_end - t_start;
    path_cost.push_back(org_graph->distance2src[org_graph->sink_id_]);
    org_graph->cur_path_max_cost = -org_graph->distance2src[org_graph->sink_id_];
    // 3rd step: update edge cost (make all weights positive)
    t_start = clock();
    org_graph->update_allgraph_weights();
    t_end = clock();
    duration[2] += t_end - t_start;

    // 2nd step: extract shortest path
    t_start = clock();
    org_graph->extract_shortest_path();
    t_end = clock();
    duration[7] += t_end - t_start;

    org_graph->path_set.push_back(org_graph->shortest_path);
    path_num++;

    std::vector<unsigned long> update_node_num;

    // 4th step: find nodes for updating
    std::vector<int> node_id4updating;
    t_start = clock();
    org_graph->find_node_set4update(node_id4updating);

    t_end = clock();
    duration[3] += t_end - t_start;
    // 5st step: rebuild org_graph
    t_start = clock();
    org_graph->flip_path();//also erase the top sinker
    t_end = clock();
    duration[9] += t_end - t_start;
    while (true) {
        // 6th step: update shortest path tree of the sub-graph
        t_start = clock();
        org_graph->update_shortest_path_tree_recursive(node_id4updating);
        printf("Iteration #%d, updated node number  %ld \n", path_num, org_graph->upt_node_num);
        //printf("%d  %ld \n", path_num, node_id4updating.size());  org_graph->max_heap_size, org_graph->num_heap_operation
        t_end = clock();
        duration[5] += t_end - t_start;

        // 8th: extract shortest path
        t_start = clock();
        org_graph->extract_shortest_path();
        t_end = clock();
        duration[7] += t_end - t_start;

        // test if stop
        double cur_path_cost = path_cost[path_num - 1] + org_graph->distance2src[org_graph->sink_id_];

        if (cur_path_cost > -0.0000001) {
            break;
        }

        path_cost.push_back(cur_path_cost);
        org_graph->cur_path_max_cost = -cur_path_cost;
        org_graph->path_set.push_back(org_graph->shortest_path);
        path_num++;

//        if (path_num == 12) {
//            cout << "check point" << std::endl;
//        }
        // 9th: update weights
        t_start = clock();
        org_graph->update_subgraph_weights(node_id4updating);
        t_end = clock();
        duration[8] += t_end - t_start;

        // 4th step: find nodes for updating
        t_start = clock();
        org_graph->find_node_set4update(node_id4updating);
        t_end = clock();
        duration[3] += t_end - t_start;

        // 10th step: rebuild the graph
        t_start = clock();
        org_graph->flip_path();
        t_end = clock();
        duration[9] += t_end - t_start;
    }

    std::cout << "Parsing time is: " << parsing_time / CLOCKS_PER_SEC << std::endl;
    double cost_sum = 0;
    double cost_sum_recalculate = 0;
    for (auto&& i : path_cost){
        cost_sum += i;
    }
//    int cnt = 1;
//    for (auto && tmpPath:org_graph->path_set){
//        double tmp_path_cost = 0;
//        int tmp_edge_id;
//
//        for(int j=0;j<tmpPath.size()-1;j++){
//            tmp_edge_id = org_graph->node_id2edge_id[node_key(tmpPath[j+1], tmpPath[j])];
//            tmp_path_cost += org_graph->edge_org_weights[tmp_edge_id];
////            printf("cost is  %.7f.\n", org_graph->edge_org_weights[tmp_edge_id]);
////            if (i == path_set.size()-2)
////                cout << j << ": "  << org_graph->edge_org_weights[tmp_edge_id] << std::endl;
//            org_graph->edge_org_weights[tmp_edge_id] *= -1;
//        }
////        printf("%d %.7f\n", cnt++, tmp_path_cost);
//        cost_sum_recalculate += tmp_path_cost;
//    }
//    unsigned long total_upt_node_num = 0;
//    for (auto&& i : update_node_num){
//        total_upt_node_num += i;
//    }
    printf("The number of paths: %ld, total cost is %.7f, final path cost %.7f.\n",
           path_cost.size() , cost_sum,  path_cost[path_cost.size()-1]);
    //cout<< "The total number of updating node: " << total_upt_node_num << std::endl;

    long double all_cpu_time = 0;
    for (int i=0; i<10; i++) {
        auto time_elapsed_ms = 1000.0 * duration[i] / CLOCKS_PER_SEC;
        all_cpu_time +=  time_elapsed_ms;
        //cout << "the " <<i+1<< " step used: " << time_elapsed_ms / 1000.0 << " s\n";
    }
//    all_cpu_time = 1000.0 * (t_end-t_start) / CLOCKS_PER_SEC;
    std::cout << "The overall time is " << all_cpu_time / 1000.0 << " s\n program ending start writing!\n";

    return 0;
}
