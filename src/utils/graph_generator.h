#pragma once
#include "../data_structures/graph.h"

class GraphGenerator {
public:
    // Random connected graph: N nodes placed in [0,width] x [0,height],
    // edges added to guarantee connectivity then randomly until edgeFactor*N total edges.
    static Graph generateRandom(int numNodes,
                                double width    = 100.0,
                                double height   = 100.0,
                                int    edgeFactor = 3,
                                unsigned int seed = 42);

    // Grid graph: rows x cols nodes, 4-directional edges, optional diagonals
    static Graph generateGrid(int rows, int cols, bool diagonals = false);

    // Save graph to file in the format expected by Graph::loadFromFile
    static bool saveToFile(const Graph& graph, const std::string& filename);
};
