#pragma once
#include "../data_structures/graph.h"
#include "../data_structures/min_heap.h"
#include "algo_result.h"

class Dijkstra {
public:
    // Uses custom MinHeap internally
    static AlgoResult run(const Graph& graph, int src, int dest);

private:
    static std::vector<int> reconstructPath(const std::vector<int>& parent, int src, int dest);
};
