#pragma once
#include "../data_structures/graph.h"
#include "algo_result.h"

class BFS {
public:
    // BFS finds shortest path by hop count (ignores weights)
    static AlgoResult run(const Graph& graph, int src, int dest);

private:
    static std::vector<int> reconstructPath(const std::vector<int>& parent, int src, int dest);
};
