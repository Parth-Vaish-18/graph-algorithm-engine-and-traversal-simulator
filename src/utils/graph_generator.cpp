#include "graph_generator.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <algorithm>

// ─── Random connected graph ──────────────────────────────────────────────────
Graph GraphGenerator::generateRandom(int numNodes,
                                     double width,
                                     double height,
                                     int    edgeFactor,
                                     unsigned int seed)
{
    Graph g(numNodes);
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> distX(0.0, width);
    std::uniform_real_distribution<double> distY(0.0, height);

    // Assign random coordinates to each node
    for (int i = 0; i < numNodes; i++)
        g.setCoords(i, distX(rng), distY(rng));

    // --- Step 1: build a random spanning tree so the graph is connected ---
    std::vector<int> shuffled(numNodes);
    for (int i = 0; i < numNodes; i++) shuffled[i] = i;
    std::shuffle(shuffled.begin(), shuffled.end(), rng);

    std::set<std::pair<int,int>> edgeSet;
    for (int i = 1; i < numNodes; i++) {
        int u = shuffled[i - 1];
        int v = shuffled[i];
        if (u > v) std::swap(u, v);
        edgeSet.insert({u, v});
        double dx = g.nodes[u].x - g.nodes[v].x;
        double dy = g.nodes[u].y - g.nodes[v].y;
        double w  = std::round(std::sqrt(dx*dx + dy*dy) * 100.0) / 100.0;
        g.addEdge(u, v, w, true);
    }

    // --- Step 2: add extra random edges up to edgeFactor * numNodes total ---
    int targetEdges = std::min(edgeFactor * numNodes,
                               numNodes * (numNodes - 1) / 2);
    int attempts = targetEdges * 10;
    std::uniform_int_distribution<int> distNode(0, numNodes - 1);

    while ((int)edgeSet.size() < targetEdges && attempts-- > 0) {
        int u = distNode(rng);
        int v = distNode(rng);
        if (u == v) continue;
        if (u > v) std::swap(u, v);
        if (edgeSet.count({u, v})) continue;

        edgeSet.insert({u, v});
        double dx = g.nodes[u].x - g.nodes[v].x;
        double dy = g.nodes[u].y - g.nodes[v].y;
        double w  = std::round(std::sqrt(dx*dx + dy*dy) * 100.0) / 100.0;
        g.addEdge(u, v, w, true);
    }

    return g;
}

// ─── Grid graph ──────────────────────────────────────────────────────────────
Graph GraphGenerator::generateGrid(int rows, int cols, bool diagonals)
{
    int numNodes = rows * cols;
    Graph g(numNodes);

    // Node id for (r, c)
    auto id = [&](int r, int c) { return r * cols + c; };

    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            g.setCoords(id(r, c), (double)c, (double)r);

    // 4-directional neighbours
    const int dr4[] = {0, 1};
    const int dc4[] = {1, 0};
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            for (int d = 0; d < 2; d++) {
                int nr = r + dr4[d];
                int nc = c + dc4[d];
                if (nr < rows && nc < cols)
                    g.addEdge(id(r, c), id(nr, nc), 1.0, true);
            }
        }
    }

    // Optional diagonal neighbours (cost √2)
    if (diagonals) {
        const int drd[] = {1,  1};
        const int dcd[] = {1, -1};
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                for (int d = 0; d < 2; d++) {
                    int nr = r + drd[d];
                    int nc = c + dcd[d];
                    if (nr >= 0 && nr < rows && nc >= 0 && nc < cols)
                        g.addEdge(id(r, c), id(nr, nc), 1.4142, true);
                }
            }
        }
    }

    return g;
}

// ─── Save to file ─────────────────────────────────────────────────────────────
bool GraphGenerator::saveToFile(const Graph& graph, const std::string& filename)
{
    // Count undirected edges (each stored twice in adjList)
    int edgeCount = 0;
    for (int u = 0; u < graph.numNodes; u++)
        for (const Edge& e : graph.adjList[u])
            if (u < e.to) edgeCount++;

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot write to " << filename << "\n";
        return false;
    }

    file << graph.numNodes << " " << edgeCount << "\n";
    for (const Node& n : graph.nodes)
        file << n.x << " " << n.y << "\n";
    for (int u = 0; u < graph.numNodes; u++)
        for (const Edge& e : graph.adjList[u])
            if (u < e.to)
                file << u << " " << e.to << " " << e.weight << "\n";

    file.close();
    std::cout << "Graph saved to " << filename << "\n";
    return true;
}
