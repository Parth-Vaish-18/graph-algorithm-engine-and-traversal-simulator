#include "graph.h"
#include <fstream>
#include <sstream>
#include <iostream>

Graph::Graph(int n) : numNodes(n) {
    nodes.resize(n);
    adjList.resize(n);
    for (int i = 0; i < n; i++) {
        nodes[i] = {i, 0.0, 0.0};
    }
}

void Graph::addEdge(int from, int to, double weight, bool undirected) {
    adjList[from].push_back({to, weight});
    if (undirected) {
        adjList[to].push_back({from, weight});
    }
}

void Graph::setCoords(int id, double x, double y) {
    nodes[id].x = x;
    nodes[id].y = y;
}

bool Graph::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << "\n";
        return false;
    }

    int n, m;
    file >> n >> m;
    numNodes = n;
    nodes.resize(n);
    adjList.resize(n);

    for (int i = 0; i < n; i++) {
        double x, y;
        file >> x >> y;
        nodes[i] = {i, x, y};
    }

    for (int i = 0; i < m; i++) {
        int u, v;
        double w;
        file >> u >> v >> w;
        addEdge(u, v, w);
    }

    file.close();
    return true;
}

void Graph::printGraph() const {
    std::cout << "Graph with " << numNodes << " nodes:\n";
    for (int i = 0; i < numNodes; i++) {
        std::cout << "Node " << i << " (" << nodes[i].x << "," << nodes[i].y << "): ";
        for (const auto& e : adjList[i]) {
            std::cout << "->" << e.to << "(w=" << e.weight << ") ";
        }
        std::cout << "\n";
    }
}
