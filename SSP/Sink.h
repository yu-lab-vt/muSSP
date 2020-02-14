#pragma once

#include <vector>
#include <algorithm>       /* fabs */
//#include<bits/stdc++.h>
#include <map>
#include <limits>
//#include <iostream>

#define FINF 1000000.0 //numeric_limits<double>::max()
#define FINFHALF FINF/2.0

class Sink
{
public:
//    class prioritize{public: bool operator ()(dist_id_pair&p1 ,dist_id_pair&p2){return fabs(p1.second - p2.second) > 0.001;}};
//    class prioritize{public: bool operator ()(std::pair<int, double>&p1 , std::pair<int, double>&p2){return p1.second > p2.second;}};
//    priority_queue< pair<int, double>, vector <std::pair<int, double>> , prioritize > sink_precursors;

// use set to save sink's precursors' distances
    std::multimap<double, int> sink_precursors;
    std::vector<double> sink_precursor_weights;
    double sink_cost_ = 0; // this can be a vector, in our framework, it it a scaler
    double sink_weight_shift = 0;
//    std::vector<bool> used_sink_precursor;
    Sink() = default;
    Sink(int n, double sink_cost);

    void sink_update_all(std::vector<double> &distance2src, int sink_id, int n);

    void sink_update_all_weight(std::vector<double> &distance2src, int sink_id, int n);


    void sink_build_precursormap(std::vector<double> &ancestor_ssd, std::vector<int> &ancestor_node_id, std::vector<int> &parent_node_id, int n);


    void sink_update_all_half(std::vector<double> distance2src, int sink_id, int n);
    void sink_update_subgraph(std::vector<int> update_node_id, std::vector<double> distance2src, int sink_id, int n);
};
