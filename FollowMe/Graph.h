#pragma once
#include "Node.h"
#include "Sink.h"
#include <stack>
#include <vector>
#include <queue>
#include <limits.h>
#include <unordered_map>
#include <iostream>
#include <algorithm>    // std::find
#include <cstring>
#include <fstream>

///
/// \brief The Graph class
///
class Graph
{
public:
        int num_nodes_;
        int num_edges_;
        int src_id_, sink_id_;
        double en_weight_, ex_weight_;

        std::vector<long double> time_test;

        std::vector<Node> V_; // save all nodes in the graph (precursor/successor/edge idx)
        std::vector<double> edge_weights;
        inline size_t node_key(int i,int j) {return (size_t) i << 32 | (unsigned int) j;}
        std::unordered_map<size_t, int> node_id2edge_id;
        std::vector<int> shortest_path;
        double precursor_queue_top_val;
        // for data validation
        std::vector<std::pair<int, int>> edge_tail_head;
        std::vector<double> edge_org_weights;
        //
        std::vector<std::vector<int>> path_set;
        std::vector<bool> node_visited, edge_visited;
        std::vector<int> node_in_visited;
        std::vector<int> parent_node_id;
        std::vector<int> ancestor_node_id;
        std::vector<double> distance2src;
        std::vector<double> ancestor_ssd; //shortest distance for a single ancestor
        std::vector<std::vector<int>> ancestors_descendants; // for each ancestor, its following nodes
        std::multimap<double, int> node_upt_waitinglist;
        std::stack<int> edge_upt_waitinglist;
        double node_upt_shift, cur_upt_shift;
        std::vector<double> nodewise_upt_shift;
        std::queue<int> tplog_queue, tmp_queue;
        std::vector<int> tplog_vec;
        long upt_node_num, max_heap_size, num_heap_operation;
        double cur_path_max_cost, cur_remain_max_distance;
        Graph() = default;
        Graph(int num_nodes, int num_edges, int src_id, int sink_id, double en_weight, double ex_weight);
        Node &get_node(int pos);
        ~Graph();

        void add_edge(int tail_id, int head_id, int edge_id, double weight);

        void shortest_path_dag();
        // A function used by shortest_path_dag
        void topologicalSortUtil(int v, std::vector<bool>& visited, std::stack<int> &Stack);

        void extract_shortest_path();
        // flip shortest path
        void flip_path();

        // update edge weights
        void update_allgraph_weights();

        /**All nodes**/
        void find_node_set4update(std::vector<int> &update_node_id);
        void update_subgraph_weights(std::vector<int> &update_node_id);

        void update_shortest_path_tree_recursive(std::vector<int> &update_node_id);
        void recursive_update_successors_distance(int curr_node_id, double curr_dist, int curr_ancestor, std::vector<int> &update_node_id4edges);
        void topologicalSort_counter_order(int v);
        void update_sink_info(std::vector<int> update_node_id);

};
