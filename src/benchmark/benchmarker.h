#pragma once
#include "../data_structures/graph.h"
#include "../algorithms/algo_result.h"
#include <vector>

class Benchmarker {
public:
    // Run all 4 algorithms on the same graph and return results
    static std::vector<AlgoResult> runAll(const Graph& graph, int src, int dest);

    // Print a formatted comparison table to stdout
    static void printComparison(const std::vector<AlgoResult>& results);
};
