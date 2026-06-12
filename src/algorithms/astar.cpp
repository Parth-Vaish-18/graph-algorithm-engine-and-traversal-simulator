#include <algorithm>
#include "astar.h"
#include <vector>
#include <queue>
#include <cmath>
#include <chrono>

double AStar::heuristic(const Node& a, const Node& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

AlgoResult AStar::run(const Graph& graph, int src, int dest) {
    auto start = std::chrono::high_resolution_clock::now();

    int n = graph.numNodes;
    std::vector<double> gScore(n, INF); // actual cost from src
    std::vector<double> fScore(n, INF); // gScore + heuristic
    std::vector<int> parent(n, -1);
    std::vector<bool> closed(n, false);
    int nodesVisited = 0;

    // Min-heap: {fScore, nodeId}
    using P = std::pair<double, int>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> openSet;

    gScore[src] = 0.0;
    fScore[src] = heuristic(graph.nodes[src], graph.nodes[dest]);
    openSet.push({fScore[src], src});

    while (!openSet.empty()) {
        auto topNode = openSet.top();
        double f = topNode.first;
        int u = topNode.second;
        openSet.pop();

        if (closed[u]) continue;
        closed[u] = true;
        nodesVisited++;

        if (u == dest) break;

        for (const Edge& e : graph.adjList[u]) {
            int v = e.to;
            if (closed[v]) continue;

            double tentativeG = gScore[u] + e.weight;
            if (tentativeG < gScore[v]) {
                gScore[v] = tentativeG;
                fScore[v] = tentativeG + heuristic(graph.nodes[v], graph.nodes[dest]);
                parent[v] = u;
                openSet.push({fScore[v], v});
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    double timeMs = std::chrono::duration<double, std::milli>(end - start).count();

    AlgoResult result;
    result.algoName = "A*";
    result.nodesVisited = nodesVisited;
    result.timeMs = timeMs;
    result.pathFound = (gScore[dest] != INF);
    result.totalCost = result.pathFound ? gScore[dest] : -1;
    result.path = result.pathFound ? reconstructPath(parent, src, dest) : std::vector<int>{};

    return result;
}

std::vector<int> AStar::reconstructPath(const std::vector<int>& parent, int src, int dest) {
    std::vector<int> path;
    for (int v = dest; v != -1; v = parent[v])
        path.push_back(v);
    std::reverse(path.begin(), path.end());
    if (path.empty() || path.front() != src) return {};
    return path;
}
