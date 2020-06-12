
#include "Graph.h"
/****shared macros****/
#define MAX(x, y) ( ( (x) > (y) ) ?  x : y )
#define MIN(x, y) ( ( (x) < (y) ) ? x : y )
#define abs(x) ( ( (x) < 0 ) ? -x : x )
#define REDUCED_EDGE_WEIGHTS(i, j, e) {\
    edge_weights[e] += distance2src[i];\
    edge_weights[e] -= distance2src[j];\
}\



Graph::Graph(int num_nodes, int num_edges, int src_id, int sink_id, double en_weight, double ex_weight) {

    num_nodes_ = num_nodes;
    num_edges_ = num_edges;
    src_id_ = src_id;
    sink_id_ = sink_id;
    en_weight_ = en_weight;
    ex_weight_ = ex_weight;
    precursor_queue_top_val = FINF;

    V_ = std::vector<Node>(num_nodes);
    for (int i = 0; i < num_nodes; i++) {// indeed this is not needed
        V_[i].price = 0;
    }
    parent_node_id.assign(num_nodes, 0);
    ancestor_node_id.assign(num_nodes, 0);
    distance2src.assign(num_nodes, FINF);

    node_visited.assign(num_nodes, false);
    // this is used after building sst, so most nodes are visited already
    /**** 0: visited,
     * 1: not visited but in waitinglist,
     * 2: not visited but not in waitinglist,
     * 3: not visited and freshly labelled
     * 4: not visited and possibly will never be used
     * -1: not visitetd and permanently never be used
    *****/
    node_in_visited.assign(num_edges, 0);
    edge_visited.assign(num_edges, false);

    // data save ancestor information
    ancestor_ssd.assign(num_nodes, FINF);
    ancestors_descendants.resize(num_nodes);

    time_test.resize(100, 0);
}

Node &Graph::get_node(int node_id) {
    return V_[node_id];
}

void Graph::add_edge(int tail_id, int head_id, int edge_id, double weight) {
    //Node tail_node = get_node(tail_id);
    //Node head_node = get_node(head_id);
    //clock_t t_start;
    //clock_t t_end;
    V_[tail_id].add_successor(head_id, edge_id, weight);
    V_[head_id].add_precursor(tail_id, edge_id, weight);

    edge_weights.push_back(weight);

//    //t_start = clock();
//    // there will be no collisions for insertion, so complexity is O(1)
//    node_id2edge_id.insert({node_key(head_id, tail_id), edge_id});
//    node_id2edge_id.insert({node_key(tail_id, head_id), edge_id});
//
//    // for results validation
//    edge_tail_head.emplace_back(std::make_pair(tail_id, head_id));
//    edge_org_weights.push_back(weight);
//
//    //t_end = clock();
//    //time_test[6] = time_test[6] + t_end - t_start;
//
//    if (edge_weights.size() - 1 != edge_id)
//        cout << "we got wrong edge number" << std::endl;
}


// A recursive function used by shortestPath. See below link for details
// https://www.geeksforgeeks.org/topological-sorting/
void Graph::topologicalSortUtil(int v, std::vector<bool>& visited, std::stack<int> &Stack) {
    // Mark the current node as visited
    visited[v] = true;

    // Recur for all the vertices adjacent to this vertex
    for (auto node_id : V_[v].successor_idx)//(int i = 0; i < this->V_[v].successor_idx.size(); ++i)
    {
        //int node_id = this->V_[v].successor_idx[i];
        if (!visited[node_id])
            topologicalSortUtil(node_id, visited, Stack);
    }
    // Push current vertex to stack which stores topological sort
    Stack.push(v);
}

/*************************
 * Build the shortest path tree from a DAG. It will only be called once
 * this shortest path tree will be saved in this->parent_node_id. their distance
 * will be saved in distance2src. their ancestors will be saved in ancestor_node_id.
 * The function to find shortest paths from given vertex. It uses recursive
 * topologicalSortUtil() to get topological sorting of given graph.
 *
 * half_flag == true means: we only need to save former node (ancestor is also former nodes)
*************************/
void Graph::shortest_path_dag() {
    std::stack<int> Stack;
    // Mark all the vertices as not visited
    std::vector<bool> visited(num_nodes_, false);

    // Call the recursive helper function to store Topological Sort
    // starting from all vertices one by one
    for (int i = 0; i < this->num_nodes_; i++)
        if (!visited[i])
            topologicalSortUtil(i, visited, Stack);

    // Initialize distances to all vertices as infinite and distance
    // to source as 0
    std::fill(distance2src.begin(), distance2src.end(), FINF);
    //memset(&distance2src[0], 1000000, distance2src.size()* sizeof(distance2src[0]));

//    for (int i = 0; i < this->num_nodes_; i++)
//        distance2src[i] = FINF;
//    cout << distance2src.size()<< std::endl;
    distance2src[src_id_] = 0;
    parent_node_id[src_id_] = 0;
    // Process vertices in topological order
    while (!Stack.empty()) {
        // Get the next vertex from topological order
        int cur_node_id = Stack.top();
        Stack.pop();

        // Update distances of all adjacent vertices
        for (size_t i = 0; i < V_[cur_node_id].successor_idx.size(); ++i) {
            int cur_succ_id = V_[cur_node_id].successor_idx[i];
            double cur_distance = distance2src[cur_node_id] + V_[cur_node_id].successor_edges_weights[i];
            if (distance2src[cur_succ_id] > cur_distance)
            {
                distance2src[cur_succ_id] = cur_distance;
                parent_node_id[cur_succ_id] = cur_node_id;
            }
        }
    }
}

// update edge weights of all graph
// can only be used when set precursors == succesors
void Graph::update_allgraph_weights()
{
    for (int i = 0; i < num_nodes_; i++) {
        //vector<int>::iterator in, ie;
//        for (in = V_[i].successor_idx.begin(), ie = V_[i].successor_edges_idx.begin();
//             in != V_[i].successor_idx.end(); in++, ie++) {
        for (size_t j = 0; j < V_[i].successor_idx.size(); j++) {
            int in = V_[i].successor_idx[j];
            int ie = V_[i].successor_edges_idx[j];
            REDUCED_EDGE_WEIGHTS(i, in, ie);//
            //edge_weights[*ie] = edge_weights[*ie] + distance2src[i] - distance2src[*in];
        }
    }
    memset(&distance2src[0], 0, distance2src.size() * sizeof(distance2src[0]));
}

void Graph::extract_shortest_path()
{
    shortest_path.clear();
    int tmp_node_id = sink_id_;
    while (tmp_node_id != src_id_) {
        shortest_path.push_back(tmp_node_id);
        tmp_node_id = parent_node_id[tmp_node_id];
    }
    shortest_path.push_back(src_id_);
}

void Graph::flip_path() { // erase the best one link to sink
    /** for 2 and end-1, specially handled ***/
    // node path(2)
    int node_tmp = shortest_path[shortest_path.size() - 1];// the path currently is from sink to src
    std::vector<int>::iterator edge_id_it;
    int tmp_ie;
    std::vector<double>::iterator edge_weight_it;
    double tmp_edge_weight;
    auto it = find(V_[node_tmp].successor_idx.begin(), V_[node_tmp].successor_idx.end(),
                   shortest_path[shortest_path.size() - 2]);
    //// erase
    auto pos = it - V_[node_tmp].successor_idx.begin();
    //V_[node_tmp].successor_edges_idx.erase(V_[node_tmp].successor_edges_idx.begin() + pos);
    tmp_edge_weight = *(V_[node_tmp].successor_edges_weights.begin() + pos);
    V_[node_tmp].successor_edges_weights.erase(V_[node_tmp].successor_edges_weights.begin() + pos);
    V_[node_tmp].successor_idx.erase(it);
    edge_id_it = V_[node_tmp].successor_edges_idx.begin() + pos;
    tmp_ie = *edge_id_it;
    V_[node_tmp].successor_edges_idx.erase(edge_id_it);

    //// add
    V_[node_tmp].precursor_edges_weights.push_back(-tmp_edge_weight);
    V_[node_tmp].precursor_idx.push_back(shortest_path[shortest_path.size() - 2]);
    V_[node_tmp].precursor_edges_idx.push_back(tmp_ie);
    // node path(end-1)
    node_tmp = shortest_path[0];
    it = find(V_[node_tmp].precursor_idx.begin(), V_[node_tmp].precursor_idx.end(), shortest_path[1]);
    pos = it - V_[node_tmp].precursor_idx.begin();
    //// erase
    tmp_edge_weight = *(V_[node_tmp].precursor_edges_weights.begin() + pos);
    V_[node_tmp].precursor_edges_weights.erase(V_[node_tmp].precursor_edges_weights.begin() + pos);
    V_[node_tmp].precursor_idx.erase(it);
    edge_id_it = V_[node_tmp].precursor_edges_idx.begin() + pos;
    tmp_ie = *edge_id_it;
    V_[node_tmp].precursor_edges_idx.erase(edge_id_it);
    //// add
    V_[node_tmp].successor_edges_weights.push_back(-tmp_edge_weight);
    V_[node_tmp].successor_idx.push_back(shortest_path[1]);
    V_[node_tmp].successor_edges_idx.push_back(tmp_ie);
    // from 3 to end - 2, reverse their precursor
    int ie;
    std::vector<int>::iterator edge_id_it_suc;
    for (unsigned long i = shortest_path.size() - 2; i >= 1; i--) {
        node_tmp = shortest_path[i];
        it = find(V_[node_tmp].precursor_idx.begin(), V_[node_tmp].precursor_idx.end(), shortest_path[i+1]);
        *it = shortest_path[i - 1];
        parent_node_id[node_tmp] = *it;
        pos = it - V_[node_tmp].precursor_idx.begin();
        edge_id_it = V_[node_tmp].precursor_edges_idx.begin()+pos;
        ie = *edge_id_it;//= node_id2edge_id[node_key(node_tmp, *it)];

        it = find(V_[node_tmp].successor_idx.begin(), V_[node_tmp].successor_idx.end(), shortest_path[i-1]);
        *it = shortest_path[i + 1];
        pos = it - V_[node_tmp].successor_idx.begin();
        edge_id_it_suc = V_[node_tmp].successor_edges_idx.begin()+pos;

        *edge_id_it = *edge_id_it_suc;
        *edge_id_it_suc = ie;//= node_id2edge_id[node_key(*it, node_tmp)];

    }

}

/*****************************************
 * if we use all nodes in the subgraph
 *
 * **************************************/
void Graph::find_node_set4update(std::vector<int> &update_node_id) {
}

void Graph::topologicalSort_counter_order(int v) {

}

void Graph::recursive_update_successors_distance(int curr_node_id, double curr_dist, int curr_ancestor,
                                                 std::vector<int> &update_node_id4edges) {

}

// use dijkstra algorithm to update the shorttest path tree
//
void Graph::update_shortest_path_tree_recursive(std::vector<int> &update_node_id)
{
    for (int i=0; i<num_nodes_; i++){
        distance2src[i] = FINF;
    }
    distance2src[0] = 0;
    node_upt_waitinglist.insert(std::make_pair(0, 0)); // insert source
    max_heap_size = node_upt_waitinglist.size();
    num_heap_operation = 0;
    // check if the pre-best-choice can still be top of the sink_precursors
    std::multimap<double, int>::iterator multi_map_it;
    int curr_node_id;
    double curr_node_dist;
    upt_node_num = 0;
    while (!node_upt_waitinglist.empty()) {
        multi_map_it = node_upt_waitinglist.begin(); //Current vertex. The shortest distance for this has been found
        curr_node_id = (*multi_map_it).second;
        curr_node_dist = (*multi_map_it).first;
        if (curr_node_id == sink_id_) {
            break;
        }
        else {
            upt_node_num ++;
        }
        node_upt_waitinglist.erase(multi_map_it); // remove the top one
        for (size_t j = 0; j < V_[curr_node_id].successor_idx.size(); j++) {
            int it = V_[curr_node_id].successor_idx[j];
            int ie = V_[curr_node_id].successor_edges_idx[j];
            double cur_edge_weight = edge_weights[ie];

            if (cur_edge_weight + curr_node_dist - distance2src[it] < -0.0000001) {
                //If this node is not visited and the current parent node distance+distance from there to this node is shorted than the initial distace set to this node, update it
                parent_node_id[it] = curr_node_id;
                distance2src[it] = cur_edge_weight + curr_node_dist;
                // no need to update ancestor in this condition
                //Set the new distance and add to map
                node_upt_waitinglist.insert(std::make_pair(cur_edge_weight + curr_node_dist, it));
                num_heap_operation ++;
                //if (max_heap_size < node_upt_waitinglist.size())
                //    max_heap_size = node_upt_waitinglist.size();
            }
        }
    }
    node_upt_waitinglist.clear();
    for (int i=0; i<num_nodes_; i++){
        if (distance2src[i] > distance2src[sink_id_])
            distance2src[i] = distance2src[sink_id_];
    }
}

void Graph::update_sink_info(std::vector<int> update_node_id) {

}

// update edge weights of subgraph
void Graph::update_subgraph_weights(std::vector<int> &update_node_id) {
    for (int i = 0; i<num_nodes_; i++) {
        for (size_t j = 0; j<V_[i].successor_idx.size(); j++) {
            int ie = V_[i].successor_edges_idx[j];
            int it = V_[i].successor_idx[j];
            REDUCED_EDGE_WEIGHTS(i, it, ie);
        }
    }
}


Graph::~Graph() {
}
