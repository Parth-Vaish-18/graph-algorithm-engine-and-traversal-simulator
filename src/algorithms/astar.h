#pragma once
#include "../data_structures/graph.h"
#include "algo_result.h"
#include <functional>

class AStar {
public:
    static AlgoResult run(const Graph& graph, int src, int dest);

private:
    static double heuristic(const Node& a, const Node& b); // Euclidean distance
    static std::vector<int> reconstructPath(const std::vector<int>& parent, int src, int dest);
};
