#include <algorithm>
#include "dijkstra.h"
#include <vector>
#include <chrono>

AlgoResult Dijkstra::run(const Graph& graph, int src, int dest) {
    auto start = std::chrono::high_resolution_clock::now();

    int n = graph.numNodes;
    std::vector<double> dist(n, INF);
    std::vector<int> parent(n, -1);
    std::vector<bool> visited(n, false);
    int nodesVisited = 0;

    MinHeap heap;
    dist[src] = 0.0;
    heap.push(0.0, src);

    while (!heap.isEmpty()) {
        HeapNode curr = heap.pop();
        int u = curr.value;

        if (visited[u]) continue;
        visited[u] = true;
        nodesVisited++;

        if (u == dest) break;

        for (const Edge& e : graph.adjList[u]) {
            int v = e.to;
            double newDist = dist[u] + e.weight;
            if (newDist < dist[v]) {
                dist[v] = newDist;
                parent[v] = u;
                heap.push(newDist, v);
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    double timeMs = std::chrono::duration<double, std::milli>(end - start).count();

    AlgoResult result;
    result.algoName = "Dijkstra";
    result.nodesVisited = nodesVisited;
    result.timeMs = timeMs;
    result.pathFound = (dist[dest] != INF);
    result.totalCost = result.pathFound ? dist[dest] : -1;
    result.path = result.pathFound ? reconstructPath(parent, src, dest) : std::vector<int>{};

    return result;
}

std::vector<int> Dijkstra::reconstructPath(const std::vector<int>& parent, int src, int dest) {
    std::vector<int> path;
    for (int v = dest; v != -1; v = parent[v])
        path.push_back(v);
    std::reverse(path.begin(), path.end());
    if (path.front() != src) return {}; // no valid path
    return path;
}
