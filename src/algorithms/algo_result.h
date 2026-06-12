#pragma once
#include <vector>
#include <string>
#include <limits>

const double INF = std::numeric_limits<double>::infinity();

struct AlgoResult {
    std::string algoName;
    std::vector<int> path;         // node IDs from src to dest
    double totalCost;
    int nodesVisited;
    double timeMs;                 // wall-clock time in milliseconds
    bool pathFound;
};
