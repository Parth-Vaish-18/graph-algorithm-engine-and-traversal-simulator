#include <algorithm>
#include "dfs.h"
#include <vector>
#include <chrono>

bool DFS::dfsHelper(const Graph& graph, int u, int dest,
                    std::vector<bool>& visited,
                    std::vector<int>& parent,
                    int& nodesVisited) {
    visited[u] = true;
    nodesVisited++;

    if (u == dest) return true;

    for (const Edge& e : graph.adjList[u]) {
        int v = e.to;
        if (!visited[v]) {
            parent[v] = u;
            if (dfsHelper(graph, v, dest, visited, parent, nodesVisited))
                return true;
        }
    }
    return false;
}

AlgoResult DFS::run(const Graph& graph, int src, int dest) {
    auto start = std::chrono::high_resolution_clock::now();

    int n = graph.numNodes;
    std::vector<bool> visited(n, false);
    std::vector<int> parent(n, -1);
    int nodesVisited = 0;

    bool found = dfsHelper(graph, src, dest, visited, parent, nodesVisited);

    auto end = std::chrono::high_resolution_clock::now();
    double timeMs = std::chrono::duration<double, std::milli>(end - start).count();

    double totalCost = 0.0;
    std::vector<int> path;
    if (found) {
        for (int v = dest; v != -1; v = parent[v])
            path.push_back(v);
        std::reverse(path.begin(), path.end());
        if (!path.empty() && path.front() == src) {
            for (int i = 0; i + 1 < (int)path.size(); i++) {
                int u = path[i], v = path[i + 1];
                for (const Edge& e : graph.adjList[u]) {
                    if (e.to == v) { totalCost += e.weight; break; }
                }
            }
        } else {
            path.clear();
            found = false;
        }
    }

    AlgoResult result;
    result.algoName = "DFS";
    result.nodesVisited = nodesVisited;
    result.timeMs = timeMs;
    result.pathFound = found;
    result.totalCost = found ? totalCost : -1;
    result.path = path;

    return result;
}
