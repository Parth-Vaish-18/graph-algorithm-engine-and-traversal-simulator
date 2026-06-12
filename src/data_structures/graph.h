#pragma once
#include <vector>
#include <string>

struct Edge {
    int to;
    double weight;
};

struct Node {
    int id;
    double x, y; // coordinates for A* heuristic
};

class Graph {
public:
    int numNodes;
    std::vector<Node> nodes;
    std::vector<std::vector<Edge>> adjList;

    Graph(int n);
    void addEdge(int from, int to, double weight, bool undirected = true);
    void setCoords(int id, double x, double y);
    bool loadFromFile(const std::string& filename);
    void printGraph() const;
};
