#pragma once
#include "../algorithms/algo_result.h"
#include "../data_structures/graph.h"
#include <vector>
#include <string>

class JsonWriter {
public:
    // Write benchmark results + graph structure to a JSON file for frontend
    static bool write(const std::string& filepath,
                      const Graph& graph,
                      int src, int dest,
                      const std::vector<AlgoResult>& results);
};
