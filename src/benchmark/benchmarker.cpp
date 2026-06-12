#include "benchmarker.h"
#include "../algorithms/dijkstra.h"
#include "../algorithms/astar.h"
#include "../algorithms/bfs.h"
#include "../algorithms/dfs.h"
#include <iostream>
#include <iomanip>
#include <chrono> // Added for the stopwatch

std::vector<AlgoResult> Benchmarker::runAll(const Graph& graph, int src, int dest) {
    std::vector<AlgoResult> results;
    int iters = 1000; // Force the algorithm to run 1000 times

    // 1. DIJKSTRA
    auto t1 = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < iters; i++) Dijkstra::run(graph, src, dest);
    auto t2 = std::chrono::high_resolution_clock::now();
    
    AlgoResult rD = Dijkstra::run(graph, src, dest);
    rD.timeMs = std::chrono::duration<double, std::milli>(t2 - t1).count() / iters;
    results.push_back(rD);

    // 2. A*
    t1 = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < iters; i++) AStar::run(graph, src, dest);
    t2 = std::chrono::high_resolution_clock::now();
    
    AlgoResult rA = AStar::run(graph, src, dest);
    rA.timeMs = std::chrono::duration<double, std::milli>(t2 - t1).count() / iters;
    results.push_back(rA);

    // 3. BFS
    t1 = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < iters; i++) BFS::run(graph, src, dest);
    t2 = std::chrono::high_resolution_clock::now();
    
    AlgoResult rB = BFS::run(graph, src, dest);
    rB.timeMs = std::chrono::duration<double, std::milli>(t2 - t1).count() / iters;
    results.push_back(rB);

    // 4. DFS
    t1 = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < iters; i++) DFS::run(graph, src, dest);
    t2 = std::chrono::high_resolution_clock::now();
    
    AlgoResult rDFS = DFS::run(graph, src, dest);
    rDFS.timeMs = std::chrono::duration<double, std::milli>(t2 - t1).count() / iters;
    results.push_back(rDFS);

    return results;
}

void Benchmarker::printComparison(const std::vector<AlgoResult>& results) {
    std::cout << "\n";
    std::cout << std::left
              << std::setw(12) << "Algorithm"
              << std::setw(12) << "Found"
              << std::setw(14) << "Cost"
              << std::setw(16) << "Nodes Visited"
              << std::setw(14) << "Time (us)"
              << "\n";
    std::cout << std::string(68, '-') << "\n";

    for (const auto& r : results) {
        std::cout << std::left
                  << std::setw(12) << r.algoName
                  << std::setw(12) << (r.pathFound ? "Yes" : "No")
                  << std::setw(14) << (r.pathFound ? std::to_string(r.totalCost) : "N/A")
                  << std::setw(16) << r.nodesVisited
                  << std::setw(14) << std::fixed << std::setprecision(3) << (r.timeMs * 1000.0)
                  << "\n";
    }
    std::cout << "\n";
}
