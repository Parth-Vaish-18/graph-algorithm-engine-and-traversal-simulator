#include <algorithm>
#include "bfs.h"
#include <queue>
#include <vector>
#include <chrono>

AlgoResult BFS::run(const Graph& graph, int src, int dest) {
    auto start = std::chrono::high_resolution_clock::now();

    int n = graph.numNodes;
    std::vector<bool> visited(n, false);
    std::vector<int> parent(n, -1);
    int nodesVisited = 0;

    std::queue<int> q;
    visited[src] = true;
    q.push(src);

    bool found = false;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        nodesVisited++;

        if (u == dest) { found = true; break; }

        for (const Edge& e : graph.adjList[u]) {
            int v = e.to;
            if (!visited[v]) {
                visited[v] = true;
                parent[v] = u;
                q.push(v);
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    double timeMs = std::chrono::duration<double, std::milli>(end - start).count();

    // Compute total cost along BFS path (sum of weights on found path)
    double totalCost = 0.0;
    std::vector<int> path;
    if (found) {
        path = reconstructPath(parent, src, dest);
        for (int i = 0; i + 1 < (int)path.size(); i++) {
            int u = path[i], v = path[i + 1];
            for (const Edge& e : graph.adjList[u]) {
                if (e.to == v) { totalCost += e.weight; break; }
            }
        }
    }

    AlgoResult result;
    result.algoName = "BFS";
    result.nodesVisited = nodesVisited;
    result.timeMs = timeMs;
    result.pathFound = found;
    result.totalCost = found ? totalCost : -1;
    result.path = path;

    return result;
}

std::vector<int> BFS::reconstructPath(const std::vector<int>& parent, int src, int dest) {
    std::vector<int> path;
    for (int v = dest; v != -1; v = parent[v])
        path.push_back(v);
    std::reverse(path.begin(), path.end());
    if (path.empty() || path.front() != src) return {};
    return path;
}
