

#include "Node.h"
#include "Sink.h"
#include <stack>
#include <vector>
#include <queue>
#include <set>
#include <limits.h>
#include <unordered_map>
#include <iostream>
#include <algorithm>    // std::find
#include <cstring>
#include <fstream>
//#include <list>

#define INF INT_MAX
//#define FINF 1000000.0 //numeric_limits<double>::max()
//#define FINFHALF FINF/2.0

#ifndef MIN_COST_FLOW_PROJ_GRAPH_H
#define MIN_COST_FLOW_PROJ_GRAPH_H


using namespace std;



class Graph{
public:

        int num_nodes_;
        int num_edges_;
        int src_id_, sink_id_;
        double en_weight_, ex_weight_;

        long double *time_test;

        vector<Node> V_; // save all nodes in the graph (precursor/successor/edge idx)
        vector<double> edge_weights;
        inline size_t node_key(int i,int j) {return (size_t) i << 32 | (unsigned int) j;}
        unordered_map<size_t, int> node_id2edge_id;
        vector<int> shortest_path;
        Sink *sink_info;
        double precursor_queue_top_val;
        // for data validation
        vector<pair<int, int>> edge_tail_head;
        vector<double> edge_org_weights;
        //
        vector<vector<int>> path_set;
        vector<bool> node_visited, edge_visited;
        vector<int> node_in_visited;
        vector<int> parent_node_id;
        vector<int> ancestor_node_id;
        vector<double> distance2src;
        vector<double> ancestor_ssd; //shortest distance for a single ancestor
        vector<vector<int>> ancestors_descendants; // for each ancestor, its following nodes
        multimap<double, int> node_upt_waitinglist;
        stack<int> edge_upt_waitinglist;
        double node_upt_shift, cur_upt_shift;
        vector<double> nodewise_upt_shift;
        queue<int> tplog_queue, tmp_queue;
        vector<int> tplog_vec;
        long upt_node_num, max_heap_size, num_heap_operation;
        double cur_path_max_cost, cur_remain_max_distance;
        Graph(){};
        Graph(int num_nodes, int num_edges, int src_id, int sink_id, double en_weight, double ex_weight);
        Node &get_node(int pos);
        ~Graph();

        class prioritize{public: bool operator ()(pair<int, double>&p1 ,pair<int, double>&p2){return p1.second>p2.second;}};

        void add_edge(int tail_id, int head_id, int edge_id, double weight);

        void shortest_path_dag();
        // A function used by shortest_path_dag
        void topologicalSortUtil(int v, bool visited[], stack<int> &Stack);

        void extract_shortest_path();
        // flip shortest path
        void flip_path();

        // update edge weights
        void update_allgraph_weights();

        /**All nodes**/
        void find_node_set4update(vector<int> &update_node_id);
        void update_subgraph_weights(vector<int> &update_node_id);

        void update_shortest_path_tree_recursive(vector<int> &update_node_id);
        void recursive_update_successors_distance(int curr_node_id, double curr_dist, int curr_ancestor, vector<int> &update_node_id4edges);
        void topologicalSort_counter_order(int v);
        void update_sink_info(vector<int> update_node_id);

};

#endif //MIN_COST_FLOW_PROJ_GRAPH_H
