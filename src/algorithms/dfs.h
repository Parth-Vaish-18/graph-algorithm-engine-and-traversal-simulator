#pragma once
#include "../data_structures/graph.h"
#include "algo_result.h"

class DFS {
public:
    static AlgoResult run(const Graph& graph, int src, int dest);

private:
    static bool dfsHelper(const Graph& graph, int u, int dest,
                          std::vector<bool>& visited,
                          std::vector<int>& parent,
                          int& nodesVisited);
};
