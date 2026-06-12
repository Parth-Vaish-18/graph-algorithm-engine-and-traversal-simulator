#include "json_writer.h"
#include <fstream>
#include <sstream>
#include <iostream>

static std::string vecToJsonArray(const std::vector<int>& v) {
    std::ostringstream ss;
    ss << "[";
    for (int i = 0; i < (int)v.size(); i++) {
        ss << v[i];
        if (i + 1 < (int)v.size()) ss << ",";
    }
    ss << "]";
    return ss.str();
}

bool JsonWriter::write(const std::string& filepath,
                       const Graph& graph,
                       int src, int dest,
                       const std::vector<AlgoResult>& results) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot write to " << filepath << "\n";
        return false;
    }

    file << "{\n";

    // Graph nodes
    file << "  \"nodes\": [\n";
    for (int i = 0; i < graph.numNodes; i++) {
        file << "    {\"id\":" << i
             << ",\"x\":" << graph.nodes[i].x
             << ",\"y\":" << graph.nodes[i].y << "}";
        if (i + 1 < graph.numNodes) file << ",";
        file << "\n";
    }
    file << "  ],\n";

    // Graph edges (deduplicated for undirected)
    file << "  \"edges\": [\n";
    bool firstEdge = true;
    for (int u = 0; u < graph.numNodes; u++) {
        for (const Edge& e : graph.adjList[u]) {
            if (u < e.to) { // only write each undirected edge once
                if (!firstEdge) file << ",\n";
                file << "    {\"from\":" << u
                     << ",\"to\":" << e.to
                     << ",\"weight\":" << e.weight << "}";
                firstEdge = false;
            }
        }
    }
    file << "\n  ],\n";

    file << "  \"src\": " << src << ",\n";
    file << "  \"dest\": " << dest << ",\n";

    // Algorithm results
    file << "  \"results\": [\n";
    for (int i = 0; i < (int)results.size(); i++) {
        const auto& r = results[i];
        file << "    {\n";
        file << "      \"name\": \"" << r.algoName << "\",\n";
        file << "      \"pathFound\": " << (r.pathFound ? "true" : "false") << ",\n";
        file << "      \"totalCost\": " << (r.pathFound ? r.totalCost : -1) << ",\n";
        file << "      \"nodesVisited\": " << r.nodesVisited << ",\n";
        file << "      \"timeMs\": " << r.timeMs << ",\n";
        file << "      \"path\": " << vecToJsonArray(r.path) << "\n";
        file << "    }";
        if (i + 1 < (int)results.size()) file << ",";
        file << "\n";
    }
    file << "  ]\n";
    file << "}\n";

    file.close();
    std::cout << "Results written to " << filepath << "\n";
    return true;
}
