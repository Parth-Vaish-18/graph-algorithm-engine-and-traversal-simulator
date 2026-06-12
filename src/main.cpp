#include <iostream>
#include <string>
#include <algorithm>
#include "data_structures/graph.h"
#include "algorithms/dijkstra.h"
#include "algorithms/astar.h"
#include "algorithms/bfs.h"
#include "algorithms/dfs.h"
#include "algorithms/algo_result.h"
#include "benchmark/benchmarker.h"
#include "utils/json_writer.h"
#include "utils/graph_generator.h"

// ─── Helpers ──────────────────────────────────────────────────────────────────

static void printUsage(const char* prog) {
    std::cout <<
        "Usage:\n"
        "  " << prog << " --graph <file>  --src <id> --dest <id> [--algo <name>] [--out <file>]\n"
        "  " << prog << " --generate <N>  --src <id> --dest <id> [--algo <name>] [--out <file>]\n"
        "  " << prog << " --grid <R> <C>  --src <id> --dest <id> [--algo <name>] [--out <file>]\n"
        "\n"
        "  --algo   dijkstra | astar | bfs | dfs | all  (default: all)\n"
        "  --out    output JSON path               (default: output/results.json)\n"
        "  --seed   RNG seed for --generate        (default: 42)\n"
        "\n"
        "Examples:\n"
        "  " << prog << " --graph graphs/sample_graph.txt --src 0 --dest 9\n"
        "  " << prog << " --generate 20 --src 0 --dest 19 --algo astar\n"
        "  " << prog << " --grid 5 5 --src 0 --dest 24 --algo all\n";
}

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

static std::string getArg(int argc, char* argv[], const std::string& flag,
                           const std::string& def = "") {
    for (int i = 1; i < argc - 1; i++)
        if (argv[i] == flag) return argv[i + 1];
    return def;
}

static bool hasFlag(int argc, char* argv[], const std::string& flag) {
    for (int i = 1; i < argc; i++)
        if (argv[i] == flag) return true;
    return false;
}

// ─── Main ─────────────────────────────────────────────────────────────────────

int main(int argc, char* argv[]) {
    if (argc < 2 || hasFlag(argc, argv, "--help") || hasFlag(argc, argv, "-h")) {
        printUsage(argv[0]);
        return 0;
    }

    // ── Parse arguments ──
    std::string algo   = toLower(getArg(argc, argv, "--algo", "all"));
    std::string outFile = getArg(argc, argv, "--out", "output/results.json");
    unsigned int seed  = (unsigned int)std::stoul(getArg(argc, argv, "--seed", "42"));

    int src  = -1;
    int dest = -1;
    try {
        src  = std::stoi(getArg(argc, argv, "--src",  "-1"));
        dest = std::stoi(getArg(argc, argv, "--dest", "-1"));
    } catch (...) {}

    if (src < 0 || dest < 0) {
        std::cerr << "Error: --src and --dest are required and must be non-negative.\n";
        printUsage(argv[0]);
        return 1;
    }

    // ── Build / load graph ──
    Graph* graphPtr = nullptr;
    Graph  ownedGraph(1);    // placeholder so we can use a reference cleanly

    if (hasFlag(argc, argv, "--graph")) {
        std::string file = getArg(argc, argv, "--graph");
        if (file.empty()) { std::cerr << "Error: --graph requires a filename.\n"; return 1; }
        ownedGraph = Graph(1);
        if (!ownedGraph.loadFromFile(file)) return 1;
        graphPtr = &ownedGraph;
        std::cout << "Loaded graph from " << file << "\n";

    } else if (hasFlag(argc, argv, "--generate")) {
        int N = std::stoi(getArg(argc, argv, "--generate", "10"));
        std::cout << "Generating random graph with " << N << " nodes (seed=" << seed << ")...\n";
        ownedGraph = GraphGenerator::generateRandom(N, 100.0, 100.0, 3, seed);
        graphPtr   = &ownedGraph;
        // Also save so frontend / reuse works
        GraphGenerator::saveToFile(ownedGraph, "graphs/generated_graph.txt");

    } else if (hasFlag(argc, argv, "--grid")) {
        // find R and C after --grid
        int rows = 5, cols = 5;
        for (int i = 1; i < argc - 2; i++) {
            if (std::string(argv[i]) == "--grid") {
                try { rows = std::stoi(argv[i+1]); cols = std::stoi(argv[i+2]); }
                catch (...) {}
                break;
            }
        }
        std::cout << "Generating " << rows << "x" << cols << " grid graph...\n";
        ownedGraph = GraphGenerator::generateGrid(rows, cols);
        graphPtr   = &ownedGraph;
        GraphGenerator::saveToFile(ownedGraph, "graphs/generated_grid.txt");

    } else {
        std::cerr << "Error: specify one of --graph, --generate, or --grid.\n";
        printUsage(argv[0]);
        return 1;
    }

    Graph& graph = *graphPtr;

    // Validate src / dest
    if (src >= graph.numNodes || dest >= graph.numNodes) {
        std::cerr << "Error: src=" << src << " or dest=" << dest
                  << " out of range (graph has " << graph.numNodes << " nodes).\n";
        return 1;
    }

    std::cout << "Graph: " << graph.numNodes << " nodes\n";
    std::cout << "Route: " << src << " -> " << dest << "\n\n";

    // ── Run algorithm(s) ──
    std::vector<AlgoResult> results;

    if (algo == "all") {
        results = Benchmarker::runAll(graph, src, dest);
    } else if (algo == "dijkstra") {
        results.push_back(Dijkstra::run(graph, src, dest));
    } else if (algo == "astar") {
        results.push_back(AStar::run(graph, src, dest));
    } else if (algo == "bfs") {
        results.push_back(BFS::run(graph, src, dest));
    } else if (algo == "dfs") {
        results.push_back(DFS::run(graph, src, dest));
    } else {
        std::cerr << "Unknown algorithm '" << algo << "'. Use: dijkstra | astar | bfs | dfs | all\n";
        return 1;
    }

    // ── Print & write results ──
    Benchmarker::printComparison(results);
    JsonWriter::write(outFile, graph, src, dest, results);

    return 0;
}
