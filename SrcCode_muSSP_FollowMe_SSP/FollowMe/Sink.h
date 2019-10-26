#include <vector>
//#include <queue>
#include <algorithm>       /* fabs */
//#include<bits/stdc++.h>
#include <map>
#include <limits>
//#include <iostream>

#define FINF 1000000.0 //numeric_limits<double>::max()
#define FINFHALF FINF/2.0

using namespace std;
#ifndef MIN_COST_FLOW_PROJ_SINK_H
#define MIN_COST_FLOW_PROJ_SINK_H



class Sink {
public:
//    class prioritize{public: bool operator ()(dist_id_pair&p1 ,dist_id_pair&p2){return fabs(p1.second - p2.second) > 0.001;}};
//    class prioritize{public: bool operator ()(pair<int, double>&p1 ,pair<int, double>&p2){return p1.second > p2.second;}};
//    priority_queue< pair<int, double>, vector <pair<int, double>> , prioritize > sink_precursors;

// use set to save sink's precursors' distances
    multimap<double, int> sink_precursors;
    vector<double> sink_precursor_weights;
    double sink_cost_ = 0; // this can be a vector, in our framework, it it a scaler
    double sink_weight_shift = 0;
//    vector<bool> used_sink_precursor;
    Sink(){};
    Sink(int n, double sink_cost);

    void sink_update_all(vector<double> &distance2src, int sink_id, int n);

    void sink_update_all_weight(vector<double> &distance2src, int sink_id, int n);


    void sink_build_precursormap(vector<double> &ancestor_ssd, vector<int> &ancestor_node_id, vector<int> &parent_node_id, int n);


    void sink_update_all_half(vector<double> distance2src, int sink_id, int n);
    void sink_update_subgraph(vector<int> update_node_id, vector<double> distance2src, int sink_id, int n);
};


#endif //MIN_COST_FLOW_PROJ_SINK_H
